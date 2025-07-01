#!/bin/bash

# 生成数据
python3 ../dataset/dag/genData.py 10 ../dataset/dag/graph.txt

# 提取 DagOpSpec 行中的 user 时间
../program --dagop ../dataset/dag/graph.txt

# 提取 DAGEnum 的执行时间
../DAGEnum ../dataset/dag/graph.txt

