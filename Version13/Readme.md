# 基于 C++17 的高性能 CPU 路径追踪渲染引擎

学习项目

原作者项目地址：https://github.com/HeaoYe/CPUPathTracing

原作者视频链接：https://www.bilibili.com/video/BV1MJYAeYEDk

## 核心摘要

本项目是一个从零手写的基于 CPU 的物理级光线追踪（Path Tracing）渲染器。项目采用 C++17 标准，脱离繁重的图形 API，仅依赖 `glm` 进行数学计算和 `rapidobj` 进行模型加载。通过历经 13 个大版本的迭代，项目实现了从单线程光线投射到多线程、SAH-BVH 加速、俄罗斯轮盘赌、多态材质系统以及复杂大场景渲染的完整路径追踪架构。

---

## 第一部分：项目核心类与模块架构分析

根据最终代码（Version13），整个项目结构可以划分为以下几个核心模块：

### 1. 核心渲染模块 (Core Rendering)

*   **`Camera`**: 相机类。负责从屏幕像素坐标（结合偏移量用于抗锯齿/随机采样）转换到 NDC 空间，再通过投影逆矩阵和视图逆矩阵生成世界空间下的 `Ray`。
*   **`Film` & `Pixel`**: 胶片与像素类。`Pixel` 保存了颜色的累加值和采样次数；`Film` 负责管理二维像素数组，支持多线程安全地写入颜色，并在最后求平均值、经过 Gamma 校正后输出为 `P6` 格式的二进制 PPM 图像。
*   **`Ray`**: 光线结构体。包含起点和方向，提供参数 $t$ 的坐标求值函数。内置了一系列用于 Debug 的可变状态（如包围盒与三角形求交次数）。
*   **`HitInfo`**: 相交信息结构体。记录光线击中物体时的 $t$ 值、交点坐标、法线方向以及命中点的 `Material` 材质指针。

### 2. 几何与场景体系 (Geometry & Scene)

*   **`Shape`**: 所有几何体的纯虚基类。规定了 `intersect`（相交测试）和 `getBounds`（获取包围盒）接口。
*   **`Sphere` / `Plane` / `Triangle`**: 基础几何体实现。`Triangle` 使用 Möller–Trumbore 算法进行高效求交；`Plane` 处理无限大平面，其包围盒设定为退化无效状态以特殊处理。
*   **`Model`**: 网格模型类。利用 `rapidobj` 解析 `.obj` 模型，将其转换为 `Triangle` 数组，并在内部封装了一个底层的 `BVH` 树进行加速。
*   **`Scene` & `ShapeInstance`**: 场景与实例管理。`ShapeInstance` 将几何体、材质与 TRS 变换矩阵（世界与局部空间相互转换）绑定。`Scene` 负责管理所有实例，并使用顶层加速结构 `SceneBVH` 进行场景级求交。

### 3. 加速结构 (Acceleration Structures)

*   **`Bounds`**: AABB 包围盒。支持点的扩展、包围盒的合并，利用预计算的 `inv_direction` 实现高效率的包围盒光线求交算法（Slab Method）。
*   **`BVH` & `BVHNode` (底层 BVH)**: 用于单个 `Model` 的加速结构。采用了线性化数组（Flatten）、32 字节对齐、12 桶 SAH（表面积启发式）等极端优化手段。
*   **`SceneBVH` & `SceneBVHNode` (顶层 BVH)**: 用于整个 `Scene` 实例的加速结构。支持将具有无限边界的物体（如平面）剥离出 BVH 树单独求交。
*   **内存分配器 (`...Allocator`)**: 专为 BVH 树节点构建设计的块分配器，每次预分配 4096 个节点，大幅降低构建时的系统内存碎片和分配耗时。

### 4. 材质与采样 (Materials & Sampling)

*   **`Material`**: 材质抽象基类。包含自发光（emissive）属性和 `sampleBRDF` 纯虚函数。
*   **`DiffuseMaterial`**: 漫反射材质。实现余弦重要性采样（Cosine Hemisphere Sampling）。
*   **`SpecularMaterial`**: 镜面反射材质。实现基于法线的完美镜面反射向量计算。
*   **`spherical.hpp`**: 提供了基于乱数序列的核心球坐标采样算法（均匀圆盘采样、均匀半球采样、余弦半球采样）。

### 5. 渲染器 (Renderers)

*   **`BaseRenderer`**: 渲染器基类。封装了多线程并发渲染的框架（`parallelFor`）、进度条更新、递增式保存图像。
*   **`NormalRenderer`**: 调试渲染器，将法线向量映射到 RGB 输出。
*   **`SimpleRTRenderer`**: 简易光线追踪（无重要性采样的基础迭代光追）。
*   **`PathTracingRenderer`**: 真正的物理级路径追踪渲染器。实现了渲染方程、基于 BRDF 的重要性采样以及**俄罗斯轮盘赌（Russian Roulette）**终止光线。
*   **`DebugRenderer`**: 用于输出 BVH 性能热力图（求交次数、栈深度）。

### 6. 多线程与基础设施 (Multithreading & Utils)

*   **`ThreadPool` & `Task`**: 自定义多线程池。支持 2D 分块（Chunk）的 `parallelFor` 并发执行。
*   **`SpinLock` & `Guard`**: 基于原子操作（`std::atomic_flag`）实现的自旋锁，极大地减少了内核态切换的开销。
*   **`Frame`**: TBN 局部坐标系构建类，用于将切线空间的采样光线转换到世界空间。
*   **`RGB`**: 负责线性空间（物理光）到 sRGB 空间（Gamma 2.2）的转换映射。
*   **`Profile`**: 基于 RAII 思想的性能计时器。
*   **`RNG`**: 基于 `std::mt19937` 的高质量随机数生成器。

---

## 第二部分：迭代过程与修改细节日志 (Iteration Log)

### Version 1：初始化项目

*   **构建系统**：搭建了基础的 CMake 构建环境，引入 C++17 标准。
*   **依赖引入**：集成 `glm` 数学库到 `thirdparty`。
*   **功能实现**：验证了环境配置，成功输出 `glm::vec3` 数据，完成 "Hello World"。

### Version 2：引入图像输出与基础多线程

*   **新增 `Film` 类**：通过分配一维 `std::vector<glm::vec3>` 管理像素。首次实现了将像素保存为 `PPM P3`（ASCII）格式的图像文件。
*   **新增 `ThreadPool` 类**：实现了一个基于 `std::list`、`std::mutex` 互斥锁、以及 `WorkerThread` 工作线程的线程池。主线程可以通过 `wait()` 阻塞等待所有任务执行完毕。

### Version 3：多线程性能优化

*   **新增 `SpinLock` 自旋锁**：使用 `std::atomic_flag` 替代操作系统级的 `std::mutex`，大幅降低任务调度时的线程阻塞开销。
*   **修改 `ThreadPool`**：引入 `std::atomic<int> alive` 替代 bool 变量。新增了核心功能 `parallelFor`，允许传入 Lambda 表达式实现并发循环。
*   **修改 `Film`**：`PPM` 输出从 `P3`（ASCII 慢速）升级为 `P6`（二进制快速）格式，并加上了 `std::clamp` 限制像素范围。加入了耗时测量统计。

### Version 4：光线、相机与相交测试

*   **新增 `Ray`、`Camera` 和 `Sphere` 类**：
    *   实现了相机视锥体定义、MVP 逆向推导（NDC 坐标反推世界坐标射线）。
    *   实现了射线与球体的求交算法（解一元二次方程）。
*   **修改 CMake**：加入 GLM 宏定义约定左手系及 NDC `[-1, 1]` 范围（`GLM_FORCE_DEPTH_ZERO_TO_ONE` 等）。
*   **修改 `ThreadPool`**：将底层任务容器从 `std::list` 更改为更适合任务调度的 `std::queue`。
*   **实现着色**：在 main 函数中，根据法线与硬编码光源方向的夹角余弦值（Lambertian），渲染出首个具备立体感的灰度球体。

### Version 5：面向对象的几何体与模型加载

*   **新增基类 `Shape`**：引入 `HitInfo` 结构体，所有几何体均继承并实现带有 `t_min` 和 `t_max` 剪枝的 `intersect` 接口。
*   **新增 `Triangle` 与 `Model` 类**：
    *   `Triangle` 实现了 Möller–Trumbore 射线-三角形相交算法。
    *   `Model` 实现了纯手写、基于 `std::ifstream` 和 `istringstream` 的简易 OBJ 文件解析（支持读取 `v`, `vn`, `f`）。
*   **修复 Bug**：修正了 `ThreadPool::wait()` 的漏洞。引入了 `pending_task_count` 原子变量，确保所有工作线程不仅取出任务，而且完全执行完毕后主线程才继续向下执行。

### Version 6：场景图管理与矩阵变换 (TRS)

*   **新增 `Plane` 类**：实现了无限大平面的求交算法。
*   **新增 `Scene` 与 `ShapeInstance`**：
    *   支持为每个物体设置位置（Pos）、缩放（Scale）、旋转（Rotate）。
    *   **核心修改**：在求交时，先将世界空间的 `Ray` 使用逆矩阵转入物体局部空间（`objectFromWorld`），求交后再将碰撞点坐标和法线使用逆转置矩阵转回世界空间。实现了场景与几何数据的解耦。

### Version 7：引入材质、多采样与基础全局光照 (GI)

*   **新增 `Material` 与 `Frame` 类**：
    *   材质开始支持反照率（albedo）、镜面反射标记（is_specular）和自发光（emissive）。
    *   `Frame` 实现 TBN 坐标系，用于在命中点基于法线构建局部反射空间。
*   **新增 `RGB` 类**：实现了线性空间到显示器空间的 Gamma 2.2 矫正。
*   **修改 `Film`**：像素存储升级为包含 `color` 累加和 `sample_count` 计数的 `Pixel` 结构，支持多次采样求均值，消除噪点。
*   **修改 渲染逻辑**：在 main 函数中废弃固定光源点，改为 `while(true)` 的迭代循环射线弹射计算。实现了基础的漫反射（半球均匀随机采样）和镜面反射，画面初步呈现全局光照效果。

### Version 8：渲染器抽象与进度监控

*   **新增 `BaseRenderer` 及子类**：将原本堆砌在 main 函数中的渲染逻辑抽离为 `NormalRenderer`（法线可视化）和 `SimpleRTRenderer`。
*   **新增 `Progress` 与 `RNG` 类**：
    *   `RNG` 封装了 `std::mt19937` 提供线程安全的随机数生成。
    *   `Progress` 提供终端渲染进度百分比打印功能。
*   **修改 渲染流程**：渲染器支持分批次渐进渲染（1->2->4->...->32 spp），并在每个批次完成后触发 `Film` 保存中间结果图像。

### Version 9：模型加载升级与并行效率优化

*   **引入 `rapidobj`**：废弃手写 OBJ 解析器，改用第三方库 `rapidobj`，解析耗时从大幅下降并提高了鲁棒性。
*   **新增 `Bounds` 包围盒**：为 `Model` 引入 AABB 包围盒（Slab Method 射线求交）。如果在根包围盒阶段未命中，直接跳过模型内部所有三角形的测试。
*   **新增 `Profile` 工具**：利用 RAII 自动打印函数的耗时。
*   **修改 `ThreadPool::parallelFor`**：引入了针对 2D 图像的 **Chunk（分块）调度** 优化。将原本按每个像素分配任务改为按图像块分配，大幅减少了互斥锁（SpinLock）争用的开销。

### Version 10：BVH 树的终极优化之旅

本章节进行了极其深入的性能优化（共经历 6 个子版本）：

*   **10.1 (基础 BVH)**：为 `Model` 引入基于空间中点的递归二叉树加速结构，相交测试从线性遍历提升为 $O(\log n)$。
*   **10.2 (展平与 32 字节对齐)**：
    *   将基于指针的 BVH 树展平（Flatten）为 `std::vector<BVHNode>` 一维数组。
    *   结构体采用 `union` 共享内存，并使用 `alignas(32)` 进行缓存行（Cache Line）对齐。
    *   用基于数组栈的 `while` 循环取代递归求交。
    *   在求交前预计算光线的 `inv_direction`，用乘法替代除法。
*   **10.3 (可视化 Debug)**：引入 `WITH_DEBUG_INFO` 宏与 Debug 渲染器体系，将包围盒遍历次数、三角形求交次数生成热力图（Heatmap）输出，用于评估 BVH 质量。
*   **10.4 (方向优化)**：在 BVH 遍历时，根据光线在分割轴上的方向符号 (`dir_is_neg`) 决定优先入栈（遍历）左节点还是右节点，大幅提升早期剪枝率。
*   **10.5 (SAH 表面积启发式算法)**：放弃基于空间几何中点的粗暴划分，采用 12 个等距桶（Buckets）在 3 个轴上分别估算划分代价（Cost = 面积 $\times$ 三角形数量），取最小值作为最优分割点。BVH 节点数增加，但渲染速度暴增。
*   **10.6 (内存与调度优化)**：
    *   引入 `BVHTreeNodeAllocator`（预分配块内存），消除了百万次 `new` 带来的堆碎片和耗时。
    *   使用 `vector::reserve` 预分配内存。
    *   **重要修改**：修改线程池，当任务队列为空时使用 `std::this_thread::sleep_for(2ms)` 放弃 CPU 时间片，解决了空转时占用大量 CPU 资源导致主线程 BVH 构建变慢的问题。

### Version 11：场景级 BVH (SceneBVH)

*   **新增 `SceneBVH` 加速结构**：将 BVH 技术从单一模型内部上升到整个场景的实例层级。
*   **修改 `Scene`**：所有的 `ShapeInstance` 现在都通过 `SceneBVH` 管理。处理了退化包围盒情况：如果物体的包围盒是无限大（如 `Plane`），将其放入 `infinity_instances` 数组，在每次射线追踪时单独与 BVH 结果作比对。
*   **修改 光追防死循环**：为 `SimpleRTRenderer` 的 `while` 循环加上了 `max_bounce_count = 32` 限制，防止因浮点数精度问题光线被困在几何体内部导致的死循环卡死。

### Version 12：标准的物理路径追踪器 (Path Tracing)

*   **新增 `PathTracingRenderer`**：基于渲染方程实现了真正的路径追踪器。
*   **新增 俄罗斯轮盘赌 (Russian Roulette)**：放弃硬编码的弹射次数限制，基于存活概率 `q=0.9` 随机终止光线，并在存活时除以概率 `q` 进行能量补偿，保证渲染结果在数学期望上的无偏性（Unbiased）。
*   **修改 `spherical.hpp`**：引入数学上严格的随机采样算法，特别是针对漫反射的**余弦重要性采样**（`CosineSampleHemisphere`），使得半球面上更靠近法线方向的光线拥有更高的采样概率，极大加速了收敛速度（降低噪点）。

### Version 13：多态材质系统构建

*   **重构 `Material` 系统**：剥离原本结构体中硬编码的分支（`if(is_specular)`），将 `Material` 变为含有纯虚函数 `sampleBRDF` 的接口。
*   **新增 `DiffuseMaterial` 与 `SpecularMaterial`**：
    *   漫反射材质实现了基于重要性采样的 BRDF 计算。
    *   镜面反射材质实现了完美反射计算。
*   **修改 场景构建逻辑**：支持更灵活的材质绑定。在 `main.cc` 终极测试中，通过随机数生成器生成了包含 10000 个不同缩放、不同材质、不同位置物体的海量场景，且配合 `SceneBVH` 和底层 `Model BVH` 实现了令人惊叹的高效渲染性能。

