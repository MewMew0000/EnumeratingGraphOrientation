# import random
#
# def generate_edge_list(n, p, output_file):
#     edges = []
#     for u in range(1, n + 1):
#         for v in range(u + 1, n + 1):  # 无向图，避免重复和自环
#             if random.random() < p:
#                 edges.append((u, v))
#
#     with open(output_file, 'w') as f:
#         for u, v in edges:
#             f.write(f"{u} {v}\n")
#
#     print(f"生成了 {len(edges)} 条边，写入文件：{output_file}")
#
# if __name__ == "__main__":
#     import argparse
#
#     parser = argparse.ArgumentParser(description="生成随机图的边列表（顶点编号从1开始）")
#     parser.add_argument("-n", "--nodes", type=int, required=True, help="图的顶点数")
#     parser.add_argument("-p", "--probability", type=float, required=True, help="边生成的概率 (0 到 1)")
#     parser.add_argument("-o", "--output", type=str, default="edges.txt", help="输出文件名")
#
#     args = parser.parse_args()
#     generate_edge_list(args.nodes, args.probability, args.output)




# import random
# import argparse
#
# def generate_edge_list(num_edges):
#     """生成随机边列表，顶点数少于边数"""
#     max_vertices = max(2, num_edges - 1)
#
#     edges = []
#     used_edges = set()
#
#     for _ in range(num_edges):
#         while True:
#             v1 = random.randint(1, max_vertices)
#             v2 = random.randint(1, max_vertices)
#
#             if v1 == v2:  # 不允许自环
#                 continue
#
#             edge = tuple(sorted([v1, v2]))  # 避免重复边
#             if edge in used_edges:
#                 continue
#
#             used_edges.add(edge)
#             edges.append((v1, v2))
#             break
#
#     return edges
#
# def main():
#     parser = argparse.ArgumentParser(description='生成随机边列表')
#     parser.add_argument('num_edges', type=int, help='要生成的边数量')
#     parser.add_argument('-o', '--output', type=str, help='输出文件名')
#
#     args = parser.parse_args()
#
#     edges = generate_edge_list(args.num_edges)
#
#     output_lines = [f"{v1} {v2}" for v1, v2 in edges]
#
#     if args.output:
#         with open(args.output, 'w') as f:
#             f.write('\n'.join(output_lines) + '\n')
#         print(f"边列表已保存到 {args.output}")
#     else:
#         for line in output_lines:
#             print(line)
#
# if __name__ == "__main__":
#     main()



import argparse

def generate_path(n, filename):
    with open(filename, 'w') as f:
        for i in range(1, n + 1):
            f.write(f"{i} {i+1}\n")
        f.write("\n")  # 文件最后空行

def main():
    parser = argparse.ArgumentParser(description="生成长度为 n 的路径边列表文件")
    parser.add_argument("n", type=int, help="路径长度，即边的数量")
    parser.add_argument("filename", type=str, help="输出文件名")
    args = parser.parse_args()

    if args.n < 1:
        print("路径长度 n 应该是正整数")
        return

    generate_path(args.n, args.filename)
    print(f"生成长度为 {args.n} 的路径边列表，已保存到 {args.filename}")

if __name__ == "__main__":
    main()

