#!/usr/bin/env python3
"""Anonymise the consolidated manuscript for double-blind submission.

Removes the author block, the running head, \thanks/\address/\email, and any
identifying name, address, ORCID or repository URL. Run by build_submission.sh,
which then verifies the produced PDF carries no identifying text.
"""
import re
import sys
import pathlib

IDENT = [
    ("Josh Bald", "Anonymous"),
    ("JOSH BALD", "ANON"),
    ("J.~Bald", "Anonymous"),
    ("jpbald93", "ANON"),
    ("@gmail.com", "@example.invalid"),
    ("@genspark.email", "@example.invalid"),
    ("0009-0002-1317-6489", "0000-0000-0000-0000"),
    ("Independent Researcher", "Anonymous"),
]


def anonymise(tex: str) -> str:
    tex = re.sub(r"\\author\{[^}]*\}", r"\\author{}", tex)
    tex = re.sub(r"\\shortauthors\{[^}]*\}", r"\\shortauthors{}", tex)
    tex = re.sub(r"\\thanks\{.*?\}\s*\n", "", tex, flags=re.S)
    tex = re.sub(r"\\address\{[^}]*\}", "", tex)
    tex = re.sub(r"\\email\{[^}]*\}", "", tex)
    for a, b in IDENT:
        tex = tex.replace(a, b)
    return tex


def main() -> int:
    src, dst = sys.argv[1], sys.argv[2]
    pathlib.Path(dst).write_text(anonymise(pathlib.Path(src).read_text(encoding="utf-8")), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
