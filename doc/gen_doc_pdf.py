#!/usr/bin/env python3
#
# Copyright (c) 2026 LunarG, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""Convert a Markdown document (with local images) to PDF.

Uses markdown-it-py to render the Markdown to HTML and PyMuPDF's Story API to lay
the HTML out into pages. Headings become PDF bookmarks, and in-document links
(e.g. a table of contents pointing at '#heading-slug') become clickable PDF links.
"""

import argparse
import io
import pathlib
import re
import sys

import pymupdf
from markdown_it import MarkdownIt

PAGE_MARGIN = 54  # points (0.75 inch)
FOOTER_FONT_SIZE = 8


def slugify(text: str) -> str:
    """Return the GitHub-style anchor for a heading text."""
    slug = text.strip().lower()
    slug = re.sub(r"[^\w\- ]", "", slug)
    return slug.replace(" ", "-")


def render_html(markdown_text: str, base_dir: pathlib.Path, content_width: float) -> str:
    """Render Markdown to HTML, adding heading ids and fitting images to the page width."""
    md = MarkdownIt("commonmark").enable("table")
    tokens = md.parse(markdown_text)

    used_slugs = {}
    for index, token in enumerate(tokens):
        if token.type == "heading_open":
            slug = slugify(tokens[index + 1].content)
            count = used_slugs.get(slug, 0)
            used_slugs[slug] = count + 1
            token.attrSet("id", slug if count == 0 else f"{slug}-{count}")
        elif token.type == "inline" and token.children:
            for child in token.children:
                if child.type != "image":
                    continue
                src = str(child.attrGet("src"))
                image_path = base_dir / src
                if not image_path.is_file():
                    raise FileNotFoundError(f"Image not found: {image_path}")
                # Assume 96 DPI images, never wider than the text column.
                natural_width = pymupdf.Pixmap(str(image_path)).width * 72 / 96
                child.attrSet("width", str(round(min(natural_width, content_width))))

    return md.renderer.render(tokens, md.options, {})


def convert(input_path: pathlib.Path, output_path: pathlib.Path, css_path: pathlib.Path,
            paper: str, footer: str) -> None:
    """Lay the document out into a PDF, then add bookmarks, links, metadata and footers."""
    page_rect = pymupdf.paper_rect(paper)
    text_rect = page_rect + (PAGE_MARGIN, PAGE_MARGIN, -PAGE_MARGIN, -PAGE_MARGIN)

    markdown_text = input_path.read_text(encoding="utf-8")
    html = render_html(markdown_text, input_path.parent, text_rect.width)
    css = css_path.read_text(encoding="utf-8") if css_path else None

    story = pymupdf.Story(html=html, archive=str(input_path.parent), user_css=css)
    positions = []
    bookmarks = []
    state = {"page": 0}

    def record_position(position):
        position.page_num = state["page"]
        positions.append(position)
        if position.open_close & 1 and 0 < position.heading <= 4:
            bookmarks.append([position.heading, position.text, state["page"]])

    stream = io.BytesIO()
    writer = pymupdf.DocumentWriter(stream)
    more = True
    while more:
        state["page"] += 1
        device = writer.begin_page(page_rect)
        more, _ = story.place(text_rect)
        story.element_positions(record_position)
        story.draw(device)
        writer.end_page()
    writer.close()

    stream.seek(0)
    doc = pymupdf.Story.add_pdf_links(stream, positions)

    # Some PyMuPDF versions silently drop content around page breaks; fail instead of shipping a truncated PDF.
    expected_images = html.count("<img ")
    placed_images = sum(len(page.get_image_info()) for page in doc)
    if placed_images != expected_images:
        raise RuntimeError(f"Only {placed_images} of {expected_images} images were laid out "
                           f"(PyMuPDF {pymupdf.VersionBind}); see doc/requirements.txt for supported versions")

    # The PDF outline requires levels to step down by at most one at a time.
    previous_level = 0
    for bookmark in bookmarks:
        bookmark[0] = min(bookmark[0], previous_level + 1)
        previous_level = bookmark[0]
    doc.set_toc(bookmarks)

    title = next((b[1] for b in bookmarks if b[0] == 1), input_path.stem)
    doc.set_metadata({"title": title, "creator": "gen_doc_pdf.py", "producer": "PyMuPDF"})

    page_count = doc.page_count
    for page in doc:
        if page.number == 0:
            continue  # no footer on the title page
        label = f"{footer}    {page.number + 1} / {page_count}" if footer else f"{page.number + 1} / {page_count}"
        width = pymupdf.get_text_length(label, fontname="helv", fontsize=FOOTER_FONT_SIZE)
        position = pymupdf.Point(page_rect.width - PAGE_MARGIN - width, page_rect.height - PAGE_MARGIN / 2)
        page.insert_text(position, label, fontname="helv", fontsize=FOOTER_FONT_SIZE, color=(0.4, 0.4, 0.4))

    output_path.parent.mkdir(parents=True, exist_ok=True)
    doc.save(str(output_path), garbage=3, deflate=True)
    doc.close()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--input", required=True, type=pathlib.Path, help="Markdown file to convert")
    parser.add_argument("--output", required=True, type=pathlib.Path, help="PDF file to write")
    parser.add_argument("--css", type=pathlib.Path, help="Stylesheet applied to the rendered HTML")
    parser.add_argument("--paper", default="letter", help="Paper size, e.g. letter or a4 (default: letter)")
    parser.add_argument("--footer", default="", help="Text shown in the page footer next to the page number")
    args = parser.parse_args()

    convert(args.input, args.output, args.css, args.paper, args.footer)
    print(f"Generated {args.output}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
