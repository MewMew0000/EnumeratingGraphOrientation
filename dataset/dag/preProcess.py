# shift_graph_vertices.py

import sys

def shift_vertex_ids(input_file, output_file):
    with open(input_file, 'r') as fin, open(output_file, 'w') as fout:
        for line in fin:
            line = line.strip()
            if not line:
                continue  # skip empty lines
            parts = line.split()
            if len(parts) != 2:
                raise ValueError(f"Invalid edge line: {line}")
            u, v = map(int, parts)
            fout.write(f"{u + 1} {v + 1}\n")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("用法: python shift_graph_vertices.py <输入文件> <输出文件>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    shift_vertex_ids(input_file, output_file)
    print(f"处理完成，已保存至: {output_file}")
