#!/usr/bin/env python3
"""Scrub a disposable supplement tree; never edit named inputs.

Lean comments (including nested block comments) are omitted, not proof terms.
Third-party dependency URLs are retained verbatim. Other text is minimally redacted;
the build separately audits the output and fails on unrecognised provenance.
"""
import pathlib
import re
import sys


def lean_without_comments(s):
    out = []
    i = 0
    in_string = False
    while i < len(s):
        if in_string:
            out.append(s[i])
            if s[i] == '\\' and i + 1 < len(s):
                i += 1
                out.append(s[i])
            elif s[i] == '"':
                in_string = False
            i += 1
        elif s.startswith('/-', i):
            depth = 1
            i += 2
            out.append(' ')
            while depth:
                if i >= len(s):
                    raise ValueError('unterminated Lean comment')
                if s.startswith('/-', i):
                    depth += 1
                    i += 2
                elif s.startswith('-/', i):
                    depth -= 1
                    i += 2
                else:
                    if s[i] == '\n':
                        out.append('\n')
                    i += 1
        elif s.startswith('--', i):
            end = s.find('\n', i)
            i = len(s) if end < 0 else end
        else:
            if s[i] == '"':
                in_string = True
            out.append(s[i])
            i += 1
    return ''.join(out)


SUBS = [
    (r'https?://github\.com/jpbald93/[A-Za-z0-9_.\-/]+', '[repository withheld]'),
    (r'10\.5281/zenodo\.\d+', '[DOI withheld]'),
    (r'Correlations\s+between\s+primitive\s+root\s+statuses\s+of\s+consecutive\s+primes', '[title withheld]'),
    (r'Cross[ -]base\s+correlations\s+of\s+Artin(?:\s+status\s+at\s+a\s+single\s+prime|\s+primes)?', '[title withheld]'),
    (r'(?:Quadratic\s+)?Exclusion\s+laws\s+for\s+consecutive\s+Artin\s+primes(?:\s+in\s+arbitrary\s+bases)?', '[title withheld]'),
    # checksum lists: keep the basename so `sha256sum -c` still names the file
    (r'(?m)^([0-9a-f]{64}\s+)\S*/home/work/.*/([^/\n]+)$', r'\1\2'),
    (r'/home/work[^\n]*', '[local path withheld]'),
    (r'Prime Math/[^\n`]*', '[local path withheld]'),
    (r'/tmp/p1_audit/', '/tmp/audit/'),
    (r'\b(?:the\s+)?Paper[\s~-]*[123]\b', 'the earlier study'),
    (r'\bJ(?:osh(?:ua)?)?\.?\s*~?Bald\b', '[name withheld]'),
    (r'\bBald\b|\bJosh(?:ua)?\b|jpbald93|0009-0002-1317-6489', '[identifier withheld]'),
    (r'GMKtec|qwen3(?::[A-Za-z0-9]+)?|\bjack\b|OpenClaw|genspark', '[provenance withheld]'),
    (r'artin[_-]correlations', 'manuscript'),
    (r'\bthe\s+author\b', 'the authors'),
]


# Lean modules named after the author's paper series are renamed in the blind copy only.
LEAN_RENAMES = [('Paper2Rebuild', 'ArbitraryBaseRebuild'), ('Paper2', 'ArbitraryBase')]


def rename_lean(root):
    art = root / 'lean' / 'Artin'
    for old, new in LEAN_RENAMES:
        f = art / f'{old}.lean'
        if f.is_file():
            f.rename(art / f'{new}.lean')
    for p in (root / 'lean').rglob('*.lean'):
        s = p.read_text(encoding='utf-8')
        t = s
        for old, new in LEAN_RENAMES:
            t = t.replace(old, new)
        if t != s:
            p.write_text(t, encoding='utf-8')


def scrub(root):
    rename_lean(root)
    count = 0
    for p in sorted(root.rglob('*')):
        if not p.is_file():
            continue
        try:
            s = p.read_text(encoding='utf-8')
        except UnicodeDecodeError:
            continue
        t = lean_without_comments(s) if p.suffix == '.lean' else s
        for pattern, replacement in SUBS:
            t = re.sub(pattern, replacement, t, flags=re.I)
        if t != s:
            p.write_text(t, encoding='utf-8')
            count += 1
    print(f'scrub: {count} text files redacted; Lean proof terms and dependency URLs retained')


if __name__ == '__main__':
    scrub(pathlib.Path(sys.argv[1]))
