import random
import argparse
from typing import List, Tuple

def generate_random_graph(n: int, p: float, output_file: str = None, directed: bool = False) -> List[Tuple[int, int]]:
    """
    生成随机图的边集合

    Args:
        n: 顶点数量 (顶点编号从1到n)
        p: 每对顶点之间存在边的概率 (0 <= p <= 1)
        output_file: 输出文件名，如果为None则只返回边列表
        directed: 是否为有向图，默认为无向图

    Returns:
        边的列表，每条边表示为 (u, v) 的元组
    """
    if not (0 <= p <= 1):
        raise ValueError("概率p必须在0和1之间")
    if n <= 0:
        raise ValueError("顶点数量必须大于0")

    edges = []

    # 遍历所有可能的顶点对
    for i in range(1, n + 1):
        start_j = i + 1 if not directed else 1  # 无向图避免重复边和自环
        for j in range(start_j, n + 1):
            if i != j and random.random() < p:  # 避免自环
                edges.append((i, j))

    # 如果指定了输出文件，则写入文件
    if output_file:
        import os
        # 获取绝对路径
        abs_path = os.path.abspath(output_file)
        with open(output_file, 'w', encoding='utf-8') as f:
            for u, v in edges:
                f.write(f"{u} {v}\n")
        print(f"图数据已保存到文件: {output_file}")
        print(f"文件绝对路径: {abs_path}")
        print(f"生成了 {len(edges)} 条边")
        print(f"当前工作目录: {os.getcwd()}")

    return edges

def print_graph_info(n: int, p: float, edges: List[Tuple[int, int]], directed: bool = False):
    """打印图的基本信息"""
    max_edges = n * (n - 1)
    if not directed:
        max_edges //= 2

    print(f"\n图信息:")
    print(f"顶点数: {n}")
    print(f"边存在概率: {p}")
    print(f"实际生成边数: {len(edges)}")
    print(f"理论最大边数: {max_edges}")
    print(f"边密度: {len(edges) / max_edges:.3f}")
    print(f"图类型: {'有向图' if directed else '无向图'}")

def main():
    parser = argparse.ArgumentParser(description='生成随机图测试数据')
    parser.add_argument('-n', '--nodes', type=int, required=True,
                        help='顶点数量')
    parser.add_argument('-p', '--probability', type=float, required=True,
                        help='边存在概率 (0-1之间)')
    parser.add_argument('-o', '--output', type=str,
                        help='输出文件名 (可选)')
    parser.add_argument('-d', '--directed', action='store_true',
                        help='生成有向图 (默认为无向图)')
    parser.add_argument('--seed', type=int,
                        help='随机种子 (用于结果复现)')

    args = parser.parse_args()

    # 设置随机种子
    if args.seed is not None:
        random.seed(args.seed)
        print(f"使用随机种子: {args.seed}")

    try:
        # 生成随机图
        edges = generate_random_graph(args.nodes, args.probability,
                                      args.output, args.directed)

        # 打印图信息
        print_graph_info(args.nodes, args.probability, edges, args.directed)

        # 如果没有指定输出文件，则在控制台显示前10条边
        if not args.output and edges:
            print(f"\n前10条边预览:")
            for i, (u, v) in enumerate(edges[:10]):
                print(f"{u} {v}")
            if len(edges) > 10:
                print("...")

    except ValueError as e:
        print(f"错误: {e}")
    except Exception as e:
        print(f"发生错误: {e}")

# 简单的交互式使用示例
def interactive_mode():
    """交互式模式"""
    print("=== 随机图生成器 ===")

    try:
        n = int(input("请输入顶点数量: "))
        p = float(input("请输入边存在概率 (0-1): "))

        output_choice = input("是否保存到文件? (y/n): ").lower()
        output_file = None
        if output_choice == 'y':
            output_file = input("请输入文件名: ").strip()
            if not output_file:
                output_file = f"graph_{n}_{p:.2f}.txt"

        directed_choice = input("是否生成有向图? (y/n): ").lower()
        directed = directed_choice == 'y'

        # 生成图
        edges = generate_random_graph(n, p, output_file, directed)

        # 显示信息
        print_graph_info(n, p, edges, directed)

        # 显示部分边
        if not output_file and edges:
            show_edges = min(10, len(edges))
            print(f"\n生成的边 (显示前{show_edges}条):")
            for u, v in edges[:show_edges]:
                print(f"{u} {v}")
            if len(edges) > show_edges:
                print("...")

    except ValueError:
        print("输入格式错误，请输入有效的数字")
    except KeyboardInterrupt:
        print("\n程序已退出")

if __name__ == "__main__":
    import sys

    # 如果有命令行参数则使用命令行模式，否则使用交互模式
    if len(sys.argv) > 1:
        main()
    else:
        interactive_mode()