---
title: STL快查
categories:
- 开发
- 语言
- C++
- STD
tags:
- STD
category_bar: true
---

# C++和c
C的printf等，封装了OS的write
C++的cout等，封装了OS的write，绕过了C的printf

# vector 数组
```
std::vector<type> v;
v.std::erase(v.begin(), v.end()); // 删除
std::reverse(v.begin(), v.end()); // 反转
sort(begin(), v.end()); // 排序 从小到大
sort(begin(), v.end(), [](int x, int y){ return x > y; }); // 排序 从大到小
```

# priority_queue 大根堆
```
std::vector<type> v;
std::priority_queue<type> p(v.begin(), v.end()); // 初始化
std::priority_queue<type, std::vector<type>, std::greater<type> v(num.begin(), num.end()); // 小根堆
```

# unordered_map 无向图
```
std::unordered_map<type, type> um;
for(auto [num, val] : um) // 结构化绑定遍历
```

