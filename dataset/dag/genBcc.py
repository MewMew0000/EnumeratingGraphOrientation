#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import random
import argparse
from typing import List, Tuple, Set

class BCCGraphGenerator:
    def __init__(self, seed=None):
        """
        初始化BCC图生成器

        Args:
            seed: 随机种子，用于复现结果
        """
        if seed is not None:
            random.seed(seed)
        self.edges = []
        self.vertex_counter = 1

    def generate_complete_graph(self, n: int) -> List[Tuple[int, int]]:
        """生成n个顶点的完全图"""
        edges = []
        vertices = list(range(self.vertex_counter, self.vertex_counter + n))
        for i in range(len(vertices)):
            for j in range(i + 1, len(vertices)):
                edges.append((vertices[i], vertices[j]))
        return edges

    def generate_cycle(self, n: int) -> List[Tuple[int, int]]:
        """生成n个顶点的环"""
        if n < 3:
            raise ValueError("环至少需要3个顶点")

        edges = []
        vertices = list(range(self.vertex_counter, self.vertex_counter + n))
        for i in range(n):
            edges.append((vertices[i], vertices[(i + 1) % n]))
        return edges

    def generate_dense_graph(self, n: int, density: float = 0.7) -> List[Tuple[int, int]]:
        """
        生成密集图

        Args:
            n: 顶点数
            density: 密度（0-1之间，1表示完全图）
        """
        max_edges = n * (n - 1) // 2
        target_edges = min(20, int(max_edges * density))

        # 先生成一个连通的生成树
        vertices = list(range(self.vertex_counter, self.vertex_counter + n))
        edges = []

        # 随机生成生成树
        available = vertices[1:]
        connected = [vertices[0]]

        while available:
            # 从已连接的顶点中选择一个
            u = random.choice(connected)
            # 从未连接的顶点中选择一个
            v = random.choice(available)
            edges.append((u, v))
            connected.append(v)
            available.remove(v)

        # 添加额外的边直到达到目标边数
        all_possible_edges = []
        for i in range(len(vertices)):
            for j in range(i + 1, len(vertices)):
                edge = (vertices[i], vertices[j])
                if edge not in edges and (edge[1], edge[0]) not in edges:
                    all_possible_edges.append(edge)

        # 随机选择额外的边
        additional_edges = min(target_edges - len(edges), len(all_possible_edges))
        if additional_edges > 0:
            extra_edges = random.sample(all_possible_edges, additional_edges)
            edges.extend(extra_edges)

        return edges

    def generate_bcc(self, bcc_type: str, size: int) -> Tuple[List[Tuple[int, int]], int]:
        """
        生成一个BCC

        Args:
            bcc_type: BCC类型 ('complete', 'cycle', 'dense')
            size: BCC大小（顶点数）

        Returns:
            (edges, bridge_vertex): 边列表和桥接顶点
        """
        start_vertex = self.vertex_counter

        if bcc_type == 'complete':
            edges = self.generate_complete_graph(size)
        elif bcc_type == 'cycle':
            edges = self.generate_cycle(size)
        elif bcc_type == 'dense':
            edges = self.generate_dense_graph(size)
        else:
            raise ValueError(f"未知的BCC类型: {bcc_type}")

        # 选择一个顶点作为桥接点
        bridge_vertex = random.randint(start_vertex, start_vertex + size - 1)

        self.vertex_counter += size
        return edges, bridge_vertex

    def generate_bcc_tree(self, num_bccs: int, bcc_configs: List[dict] = None) -> List[Tuple[int, int]]:
        """
        生成BCC树结构的图

        Args:
            num_bccs: BCC数量
            bcc_configs: BCC配置列表，每个配置包含 {'type': str, 'size': int}
        """
        if bcc_configs is None:
            # 默认配置
            bcc_configs = []
            for i in range(num_bccs):
                bcc_type = random.choice(['complete', 'cycle', 'dense'])
                if bcc_type == 'complete':
                    # 完全图的边数限制
                    max_size = int((1 + (1 + 8 * 20)**0.5) / 2)  # 解方程 n(n-1)/2 <= 20
                    size = random.randint(3, min(max_size, 8))
                elif bcc_type == 'cycle':
                    size = random.randint(3, 15)
                else:  # dense
                    size = random.randint(4, 10)

                bcc_configs.append({'type': bcc_type, 'size': size})

        all_edges = []
        bridge_vertices = []

        # 生成每个BCC
        for config in bcc_configs:
            edges, bridge_vertex = self.generate_bcc(config['type'], config['size'])
            all_edges.extend(edges)
            bridge_vertices.append(bridge_vertex)

            print(f"生成BCC: 类型={config['type']}, 大小={config['size']}, "
                  f"边数={len(edges)}, 桥接点={bridge_vertex}")

        # 连接BCC形成树结构
        if len(bridge_vertices) > 1:
            # 生成树的边
            available = bridge_vertices[1:]
            connected = [bridge_vertices[0]]

            while available:
                u = random.choice(connected)
                v = random.choice(available)
                all_edges.append((u, v))
                connected.append(v)
                available.remove(v)
                print(f"添加桥边: {u} - {v}")

        return all_edges

    def save_to_file(self, edges: List[Tuple[int, int]], filename: str):
        """保存边到文件"""
        with open(filename, 'w') as f:
            for u, v in edges:
                f.write(f"{u} {v}\n")
        print(f"图已保存到 {filename}")
        print(f"总顶点数: {self.vertex_counter - 1}")
        print(f"总边数: {len(edges)}")

def main():
    parser = argparse.ArgumentParser(description='生成BCC树结构的图')
    parser.add_argument('--num-bccs', type=int, default=5, help='BCC数量')
    parser.add_argument('--output', type=str, default='graph.txt', help='输出文件名')
    parser.add_argument('--seed', type=int, help='随机种子')
    parser.add_argument('--config', type=str, help='配置文件路径（可选）')

    args = parser.parse_args()

    generator = BCCGraphGenerator(seed=args.seed)

    # 可以在这里自定义BCC配置
    custom_configs = [
        {'type': 'complete', 'size': 4},  # K4，6条边
        {'type': 'cycle', 'size': 6},     # C6，6条边
        {'type': 'dense', 'size': 7},     # 密集图，约10-15条边
        {'type': 'complete', 'size': 5},  # K5，10条边
        {'type': 'cycle', 'size': 8},     # C8，8条边
    ]

    # 如果指定了BCC数量但没有自定义配置，则使用随机配置
    if len(custom_configs) != args.num_bccs:
        custom_configs = None

    edges = generator.generate_bcc_tree(args.num_bccs, custom_configs)
    generator.save_to_file(edges, args.output)

    # 输出统计信息
    print("\n=== 统计信息 ===")
    bcc_edges = {}
    for i, (u, v) in enumerate(edges):
        print(f"边 {i+1}: {u} - {v}")

if __name__ == "__main__":
    # 如果直接运行脚本，使用默认参数
    if len(__import__('sys').argv) == 1:
        print("=== 直接运行模式 ===")
        print("正在生成示例图...")

        generator = BCCGraphGenerator(seed=42)

        # 示例配置
        configs = [
            {'type': 'complete', 'size': 4},  # K4，6条边
            {'type': 'cycle', 'size': 5},     # C5，5条边
            {'type': 'dense', 'size': 6},     # 密集图
            {'type': 'complete', 'size': 3},  # K3，3条边
        ]

        edges = generator.generate_bcc_tree(4, configs)
        generator.save_to_file(edges, 'example_graph.txt')

        print("\n=== 生成完成 ===")
        print("文件已保存为: example_graph.txt")
        print("你可以直接用这个文件测试你的Tarjan算法！")
    else:
        main()