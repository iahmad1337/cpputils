#!/usr/bin/env python3

from pathlib import Path
from typing import Iterator
import os

WORK_DIR = Path('.')

# hardcoded topsort of includes
INCLUDE_ORDER = [
    'cpputils/common.hh',
    'cpputils/meta.hh',
    'cpputils/string.hh',
    'cpputils/debug.hh',
    'cpputils/linalg.hh',
    'cpputils/reflect.hh',
]

RESULT_NAME = 'cpputils.gen.hh'


def files_with_suffix(path: Path, suffix: str) -> Iterator[Path]:
    # os.walk is used because Path.walk is only available in 3.12+
    for root, _, files in os.walk(path):
        for file in files:
            if file.endswith(suffix):
                yield Path(root) / file


def remove_includes(text: str) -> str:
    '''
    Remove all unnecessary preprocessor derectives from file
    '''
    lines = []
    for line in text.splitlines():
        if (
            ('#include' in line and 'cpputils/' in line)
            or '#pragma once' in line
        ):
            continue
        lines.append(line)
    return os.linesep.join([
        '// START OF FILE',
        *lines,
        '// END OF FILE',
    ])


def test_relevance():
    includes = list(files_with_suffix(WORK_DIR / 'include', '.hh'))
    assert len(includes) == len(INCLUDE_ORDER), (
        "Include order must contain all include files"
    )
    for expected, got in zip(sorted(INCLUDE_ORDER), sorted(includes)):
        expected = Path('include') / expected
        assert expected == got, (
            f'Mismatch with include order: {(expected)} != {(got)}'
        )


def main():
    if not (WORK_DIR / 'make_header.py').exists():
        print('ERROR: Please launch the script in cpputils root')
        exit(1)

    test_relevance()

    includes = [Path('include') / include for include in INCLUDE_ORDER]
    srcs = list(files_with_suffix(WORK_DIR / 'src', '.cc'))

    Path(RESULT_NAME).write_text(
        os.linesep.join(
            remove_includes(Path(include).read_text()) for include in includes
        )
        + os.linesep
        + os.linesep.join(
            remove_includes(Path(src).read_text()) for src in srcs
        )
    )
    print(f'Result written to {RESULT_NAME}')


if __name__ == '__main__':
    main()
