# AutoUpdate

本项目是一款基于MFC实现的自动更新模块

[![license](https://img.shields.io/badge/license-Apache%202-brightgreen.svg?style=flat)](https://github.com/MFCer/AutoUpdate/blob/master/LICENSE)

# 特点

- 基于HTTP文件服务器，多线程下载
- 提供配置工具
- 支持多国语言（默认英文、简体中文、繁体中文）
- 极简设计，仅气泡交互（更新进度和状态等）
- 提供默认内嵌类，即插即用
- 关于内部细节：
	- 支持全屏检测，默认无全屏应用工作时提示更新
	- 支持自动更新程序自更新
	- 支持文件版本、文件HASH、更新时间等多种检测维度
	- 支持执行、覆盖、注册、解压多种更新策略
	- 支持占用文件的解锁([unlocker项目赋能](https://github.com/ez8-co/unlocker))

# 项目概览

## AutoUpdateConfig

![](https://github.com/MFCer/AutoUpdate/blob/master/doc/AutoUpdateConfig.jpg)

- 自动更新配置程序：用于生成服务端配置供客户端拉取比对是否需要更新

## AutoUpdate

- 自动更新模块：主要完成下载和升级等核心功能，用于放置在软件安装包目录内，供宿主程序调用

## AutoUpdateImplClass

- 自动更新模块嵌入类：主要提供即插即用功能，放置到你自己的工程即可，该工程单独使用是无法直接译通过的

# 联系我们

<a href="https://jq.qq.com/?_wv=1027&k=5HtUqaf"><img src="https://img.shields.io/badge/QQ群-67010755-blue.svg" /></a>
