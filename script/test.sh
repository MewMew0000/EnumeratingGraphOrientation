#!/bin/bash

# 生成数据
python3 ../dataset/dag/genData.py 260 ../dataset/dag/graph.txt

# 提取 DagOpSpec 行中的 user 时间
user_time=$(../program --dagsimpl --show ../dataset/dag/graph.txt \
    | grep 'DagOpSpec' \
    | sed -E 's/.* ([0-9.]+)s user.*/\1/')

# 提取 DAGEnum 的执行时间
exec_time=$(../DAGEnum ../dataset/dag/graph.txt \
    | grep -oE 'execute time: [0-9]+' \
    | awk '{print $3}')

# 输出结果
echo "program user time: ${user_time}s"
echo "DAGEnum execute time: ${exec_time} ms"
