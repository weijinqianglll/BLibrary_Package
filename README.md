# BLibrary 插件

## 简介
解决UE5无法对UI截图的问题，并提供多个蓝图函数方便上传图片的服务器，或保存图片到本地磁盘。

## 版本
- 版本号: 1.0
- 作者: LALOLE

## 安装方法

1. 将 `BLibrary` 文件夹复制到你的 UE5 项目的 `Plugins` 目录下
2. 如果 `Plugins` 目录不存在，请先创建该目录
3. 重启 UE 编辑器，插件会自动加载

## 目录结构
```
BLibrary/
├── BLibrary.uplugin          # 插件配置文件
├── Source/                   # 源代码
│   └── BLibrary/
│       ├── BLibrary.Build.cs
│       ├── Private/
│       └── Public/
└── Content/                  # 蓝图资源
    └── AC_PostManager.uasset
```

## 使用说明

插件安装后，你可以在蓝图中使用 BLibrary 提供的功能。

## 兼容性
- Unreal Engine 5.6
- Windows 64位

## 许可证
免费使用

## 联系方式
如有问题或建议，请联系作者。

## 更新日志
### v1.0
- 初始版本发布
