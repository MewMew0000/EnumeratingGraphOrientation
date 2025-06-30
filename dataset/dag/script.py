from collections import defaultdict

def find_bridges_and_bcc(n, edges):
    graph = defaultdict(list)
    for u,v in edges:
        graph[u].append(v)
        graph[v].append(u)

    time = 0
    disc = [-1]*(n+1)
    low = [-1]*(n+1)
    bridges = []

    def dfs(u, parent):
        nonlocal time
        disc[u] = low[u] = time
        time += 1
        for v in graph[u]:
            if disc[v] == -1:
                dfs(v, u)
                low[u] = min(low[u], low[v])
                if low[v] > disc[u]:
                    bridges.append((u,v))
            elif v != parent:
                low[u] = min(low[u], disc[v])

    for i in range(1, n+1):
        if disc[i] == -1 and i in graph:
            dfs(i, -1)

    bridge_set = set()
    for u,v in bridges:
        bridge_set.add((u,v))
        bridge_set.add((v,u))

    visited = [False]*(n+1)
    bcc_count = 0

    def dfs_bcc(u):
        stack = [u]
        visited[u] = True
        while stack:
            node = stack.pop()
            for w in graph[node]:
                if not visited[w] and (node, w) not in bridge_set:
                    visited[w] = True
                    stack.append(w)

    for i in range(1, n+1):
        if i in graph and not visited[i]:
            dfs_bcc(i)
            bcc_count += 1

    return bridges, bcc_count

def read_edges_from_file(filename):
    edges = []
    max_node = 0
    with open(filename, 'r') as f:
        for line in f:
            if line.strip():
                u,v = map(int, line.strip().split())
                edges.append((u,v))
                max_node = max(max_node, u, v)
    return edges, max_node

def write_edges_to_file(edges, filename):
    with open(filename, 'w') as f:
        for u,v in edges:
            f.write(f"{u} {v}\n")

def delete_edges_to_target_bcc(filename, target_bcc):
    edges, n = read_edges_from_file(filename)
    print(f"读入 {len(edges)} 条边，最大顶点编号 {n}")

    current_edges = edges.copy()
    deleted_edges = []

    while True:
        bridges, bcc_num = find_bridges_and_bcc(n, current_edges)
        print(f"当前桥边数: {len(bridges)}, 当前BCC数: {bcc_num}")
        if bcc_num >= target_bcc:
            print(f"已达到目标BCC数：{bcc_num} >= {target_bcc}")
            break
        if not bridges:
            print("无桥边可删除，无法继续增加BCC数。")
            break

        # 删除一条桥边（这里简单删除第一个）
        edge_to_remove = bridges[0]
        # 同时删除 (u,v) 和 (v,u) 都存在的边
        u, v = edge_to_remove
        # 删除边时需要删掉无向边，所以两端都删
        current_edges = [e for e in current_edges if e != (u,v) and e != (v,u)]
        deleted_edges.append(edge_to_remove)
        print(f"删除边 {edge_to_remove}，尝试增加BCC数...")

    # 写出结果
    write_edges_to_file(current_edges, "output_edges.txt")
    print(f"删除了 {len(deleted_edges)} 条边，结果写入 output_edges.txt")
    print("删除的边如下：")
    for e in deleted_edges:
        print(e)

# 下面是脚本入口，你可以改成命令行参数或直接修改
if __name__ == "__main__":
    input_file = "graph.txt"    # 你的输入边文件名
    target_bcc = 5              # 目标BCC数目
    delete_edges_to_target_bcc(input_file, target_bcc)
