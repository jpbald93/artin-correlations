#!/usr/bin/env python3
"""Assemble the consolidated Artin-correlations paper from Paper 1 + Paper 3.

Technical core sections are copied VERBATIM (byte-for-byte) so no numeral is retyped.
Only the section titles of two sections are edited, and P3's colliding labels are renamed.
"""
import re, sys, hashlib, pathlib

# Source locations. Override with PAPER1_TEX / PAPER3_TEX / CONSOLIDATED_DIR if the
# papers live elsewhere; the defaults match the author's workspace layout.
import os
BASE = os.environ.get("MATH_AUDIT_BASE", "/home/work/.openclaw/workspace/Prime Math")
P1 = os.environ.get("PAPER1_TEX", os.path.join(BASE, "Paper 1 Full file/paper/consecutive_artin.tex"))
P3 = os.environ.get("PAPER3_TEX", os.path.join(BASE, "Paper 3 Full file/paper/crossbase_artin.tex"))
W  = pathlib.Path(os.environ.get("CONSOLIDATED_DIR", os.path.join(BASE, "consolidated")))

p1 = open(P1, encoding="utf-8").read()
p3 = open(P3, encoding="utf-8").read()

def between(s, a, b):
    i = s.index(a); j = s.index(b, i+len(a))
    return s[i:j]

def section_with_head(s, head, nxt):
    """from the \\section line through to just before the next section."""
    i = s.index(head)
    j = s.index(nxt, i+len(head))
    return s[i:j]

def body_after_head(s, head, nxt):
    """from just after the \\section + \\label lines, to just before the next section."""
    blk = section_with_head(s, head, nxt)
    # drop the first two lines (\section{...} and \label{...})
    lines = blk.split("\n", 2)
    return lines[2] if len(lines) > 2 else ""

def bib(s):
    i = s.index(r"\begin{thebibliography}")
    j = s.index(r"\end{thebibliography}") + len(r"\end{thebibliography}")
    return s[i:j]

def bibitems(s):
    blk = bib(s)
    return re.findall(r"\\bibitem(?:\[[^\]]*\])?\{([^}]+)\}[\s\S]*?(?=\\bibitem|\\end\{thebibliography\})", blk)

# ---------------- extract verbatim blocks ----------------
s_excl   = section_with_head(p1, r"\section{The exclusion law}",            r"\section{Data and computation}")
s_glob   = section_with_head(p1, r"\section{The global anticorrelation}",    r"\section{Gap structure}")
s_gaps   = section_with_head(p1, r"\section{Gap structure}",                r"\section{Residue conditioning}")
s_resid  = section_with_head(p1, r"\section{Residue conditioning}",         r"\section{The intermediate link")
s_omega  = section_with_head(p1, r"\section{The intermediate link",         r"\section{Open questions}")
s_quest  = section_with_head(p1, r"\section{Open questions}",               r"\section{Discussion}")
d1_body  = body_after_head(p1, r"\section{Data and computation}",           r"\section{The global anticorrelation}")

s_pair   = section_with_head(p3, r"\section{The same-prime exclusion laws}", r"\section{Data and computation}")
s_cross  = section_with_head(p3, r"\section{The cross-base correlation}",    r"\section{Decomposition:")
s_decomp = section_with_head(p3, r"\section{Decomposition:",                r"\section{The residual")
s_resid3 = section_with_head(p3, r"\section{The residual",                   r"\section{Comparison with the predicted")
s_model  = section_with_head(p3, r"\section{Comparison with the predicted",  r"\section{Discussion}")
d3_body  = body_after_head(p3, r"\section{Data and computation}",            r"\section{The cross-base correlation}")

# retitle the two sections that need distinct titles in the merged paper
s_excl = s_excl.replace(r"\section{The exclusion law}", r"\section{The consecutive-prime exclusion law}", 1)

# ---------------- rename P3's colliding labels ----------------
REN = [("sec:theorem", "sec:pairlaw"),
       ("sec:decomposition", "sec:decomposition_crossbase"),
       ("tab:decomp", "tab:decomp_crossbase")]
def fix3(t):
    # P3 uses \Art_a for the SET of Artin primes; P1 uses \Art_n for the INDICATOR.
    # Render P3's set notation as \Acal_a to keep the two apart.
    t = t.replace("\\Art", "\\Acal")
    for a, b in REN:
        t = t.replace("\\label{%s}" % a, "\\label{%s}" % b)
        t = t.replace("\\ref{%s}" % a, "\\ref{%s}" % b)
        t = t.replace("\\eqref{%s}" % a, "\\eqref{%s}" % b)
    return t

s_pair, s_cross, s_decomp, s_resid3, s_model = map(fix3, (s_pair, s_cross, s_decomp, s_resid3, s_model))
d3_body = fix3(d3_body)

# --- self-citation repair: Paper 1 (BaldI) IS this paper now ---
SELF_OLD = ("The sign is the opposite of the consecutive-prime correlation measured in the\n"
            "first two papers of the series~\\cite{BaldI, BaldII} (where $\\delta < 0$ uniformly), and the")
SELF_NEW = ("The sign is the opposite of the consecutive-prime correlation measured above\n"
            "(\\S\\ref{sec:global}, where $\\delta < 0$), and the")
if SELF_OLD in s_cross:
    s_cross = s_cross.replace(SELF_OLD, SELF_NEW, 1)
else:
    sys.exit("FATAL: self-citation block not found verbatim - inspect before assembling")
DROP_BIB = ["BaldI"]

# ---------------- merged bibliography ----------------
b1 = bib(p1); b3 = bib(p3)
i1 = re.findall(r"\\bibitem(?:\[[^\]]*\])?\{([^}]+)\}", b1)
i3 = re.findall(r"\\bibitem(?:\[[^\]]*\])?\{([^}]+)\}", b3)
shared = [k for k in i1 if k in i3]
blocks = {}
for blk, keys in ((b1, i1), (b3, i3)):
    for k in keys:
        m = re.search(r"\\bibitem(?:\[[^\]]*\])?\{%s\}[\s\S]*?(?=\\bibitem|\\end\{thebibliography\})" % re.escape(k), blk)
        if m and k not in blocks:
            blocks[k] = m.group(0).rstrip() + "\n"
order = [k for k in i1] + [k for k in i3 if k not in i1]
order = [k for k in order if k not in DROP_BIB]

preamble = r"""\documentclass[11pt,reqno]{amsart}
\usepackage[margin=1.15in]{geometry}
\usepackage{amsmath,amssymb,amsthm,mathtools}
\usepackage{graphicx}
\usepackage{booktabs}
\usepackage[colorlinks=true,linkcolor=blue,citecolor=blue,urlcolor=blue]{hyperref}
\usepackage{microtype}
\usepackage{xurl}
\raggedbottom

\theoremstyle{plain}
\newtheorem{theorem}{Theorem}
\newtheorem{proposition}[theorem]{Proposition}
\newtheorem{corollary}[theorem]{Corollary}
\newtheorem{lemma}[theorem]{Lemma}
\newtheorem{conjecture}[theorem]{Conjecture}
\theoremstyle{definition}
\newtheorem{definition}[theorem]{Definition}
\newtheorem{remark}[theorem]{Remark}
\newtheorem{observation}[theorem]{Observation}

\DeclareMathOperator{\sqf}{sqf}
\newcommand{\Art}{\mathrm{Art}}
\newcommand{\Acal}{\mathcal{A}}
\newcommand{\Z}{\mathbb{Z}}
\newcommand{\Q}{\mathbb{Q}}
\newcommand{\Lsym}[2]{\left(\tfrac{#1}{#2}\right)}
\newcommand{\E}{\mathbb{E}}

\begin{document}
"""

parts = [
    preamble,
    (W/"FRONT.tex").read_text(encoding="utf-8"),
    "\n", s_excl, "\n", s_pair, "\n",
    "\\section{Data and computation}\n\\label{sec:data}\n\n"
    "The two axes of this paper use two censuses over the same primes, with slightly "
    "different exclusions at the low end, by design. We describe each.\n\n"
    "\\subsection*{The consecutive-prime census (base $10$)}\n",
    d1_body, "\n",
    "\\subsection*{The cross-base census}\n",
    d3_body, "\n",
    s_glob, "\n", s_gaps, "\n", s_resid, "\n", s_omega, "\n",
    s_cross, "\n", s_decomp, "\n", s_resid3, "\n", s_model, "\n",
    (W/"SECTION_content.tex").read_text(encoding="utf-8"),
    "\n",
    (W/"DISC.tex").read_text(encoding="utf-8"),
    "\n",
    s_quest, "\n",
    "\\begin{thebibliography}{99}\n\n" + "\n".join(blocks[k] for k in order) + "\n\\end{thebibliography}\n\n\\end{document}\n",
]
out = "\n".join(parts)
# --- merge-time citation hygiene: the gap law is proved IN this paper (Theorem thm:exclusion),
# so P3's citation of the predecessor preprint for it becomes an in-paper reference.
for _old, _new in [
    ("proved in~\\cite{BaldII}: both are", "proved above (Theorem~\\ref{thm:exclusion}); the arbitrary-base form is~\\cite{BaldII}: both are"),
    ("Like the gap exclusion law of~\\cite{BaldII}, both statements are",
     "Like the gap exclusion law of Theorem~\\ref{thm:exclusion} above, both statements are"),
]:
    if _old in out:
        out = out.replace(_old, _new); print("  merge-time citation repair:", _old[:40])
(W/"artin_correlations.tex").write_text(out, encoding="utf-8")

print("  wrote artin_correlations.tex: %d bytes, %d lines" % (len(out), out.count("\n")))
print("  bibitems merged: P1=%d P3=%d shared=%d total=%d" % (len(i1), len(i3), len(shared), len(order)))
print("  P3-only kept:", [k for k in i3 if k not in i1])
print("  BaldI cited in kept P3 blocks?", any("cite{BaldI" in t or "cite{BaldI," in t for t in (s_pair,s_cross,s_decomp,s_resid3,s_model,d3_body)))
print("  sha256(tex):", hashlib.sha256(out.encode()).hexdigest()[:16])
