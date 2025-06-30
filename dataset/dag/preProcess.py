import sys

def shift_vertex_ids(input_file, output_file):
    with open(input_file, 'r') as fin, open(output_file, 'w') as fout:
        for line in fin:
            line = line.strip()
            if not line:
                continue
            u, v = map(int, line.split())
            fout.write(f"{u + 1} {v + 1}\n")

def main():
    if len(sys.argv) != 3:
        print("用法: python shift_vertex_ids.py <输入文件> <输出文件>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    shift_vertex_ids(input_file, output_file)
    print(f"处理完成，已保存至: {output_file}")

if __name__ == "__main__":
    main()
