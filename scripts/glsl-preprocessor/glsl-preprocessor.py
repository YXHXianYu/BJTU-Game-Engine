import os
import re

INCLUDE_RE = re.compile(r'^#include "(.*?)".*$')


class CompileError(Exception):
    def __init__(self, msg):
        self.msg = msg
        super().__init__(self.msg)


cache = {}


def process(path, parents=[]):
    if path in cache.keys():
        return cache.get(path)

    if len(parents) == 0:
        parents = [path]

    # dir = os.path.dirname(path) # for include relative to cur file

    lines = open(path, 'r', encoding='utf-8').readlines()
    for (i, line) in enumerate(lines):
        res = INCLUDE_RE.match(line)
        if res is not None:
            target_path = res.group(1)
            # for include relative to ./
            target_path = os.path.normpath(os.path.join(shaders_root, target_path))
            # for include relative to cur file
            # target_path = os.path.normpath(os.path.join(dir, target_path))

            if not os.path.exists(target_path):
                raise CompileError(f"{os.path.basename(path)}:{i+1}: error: '{target_path}' file not found")
            if target_path in parents:
                raise CompileError(f"{os.path.basename(path)}:{i+1}: error: '{target_path}' recursive include")

            target_content = process(target_path, parents + [target_path])
            lines[i] = target_content

    cache[path] = ''.join(lines)
    return cache[path]


# !!! this is the root of shaders relative to this script
script_dir = os.path.dirname(os.path.abspath(__file__))
REL_ROOT = '../../engine/shader/'
# REL_ROOT = './glsl-precompiler-test/'

# the root of shaders
shaders_root = os.path.join(script_dir, REL_ROOT)
# the output dir
output_dir = os.path.join(shaders_root, './build/')

EXTS = ['.glsl', '.frag', '.vert']

if __name__ == '__main__':
    for root, dirs, files in os.walk(shaders_root):
        if os.path.relpath(root, shaders_root).startswith('build'):
            continue

        for file in files:
            ext = os.path.splitext(file)[-1]
            if ext not in EXTS:
                continue

            srcpath = os.path.normpath(os.path.join(root, file))
            relpath = os.path.relpath(srcpath, shaders_root)

            dstpath = os.path.normpath(os.path.join(output_dir, relpath))
            dstdir = os.path.dirname(dstpath)
            print(f'processing {relpath} to {os.path.relpath(dstpath, shaders_root)}...')

            if not os.path.exists(dstdir):
                os.makedirs(dstdir)

            with open(dstpath, 'w') as f:
                res = process(srcpath)
                f.write(res)
