---
layout: post
title:  fastboot source code analysis
category: Graphic
---

# 前言 #

MIT的作业做到第六个的时候，我暂时放弃了，原因有两个，第一，我认为了解了基本的光线求交的原理，以及对光反射，折射，阴影的绘制的了解，基本算是遁入空门；第二，第六个作业里的Voxel rendering还是有些难，所以就偷懒了。为了固化知识，还是硬着头皮来写吧。这个作业里主要包括Perspective Camera的实现，基本的光源渲染，还有三角形和平面的实现。

# Perspective Camera的实现 #

其实对比上一章节的平行投影，这个应该叫做透视投影。在这种投影中，光线的传播方向不是平行的，相对于显示矩形上不同的坐标，方向不同。距离显示矩形上中心一段距离的一个位置，我们叫做视点，这一段距离叫做焦距。生成每个像素的射线的方向就是：从这个视点出发到显示矩形上的任意点的向量的方向，如图：

![perspective_camera.png](http://groups.csail.mit.edu/graphics/classes/6.837/F04/assignments/assignment2/perspective_camera.png "perspective_camera.png")



# Diffuse shading #

# Triangle类和Plane类的实现 #

# 尾声 #

# 参考 #