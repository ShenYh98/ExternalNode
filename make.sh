#!/bin/bash

# 检查脚本参数
if [ "$1" != "x86" ] && [ "$1" != "clean" ] && [ "$1" != "arm" ]; then
  echo "请输入一个参数，可选参数为 x86 或 arm 或 clean"
  exit 1
fi

# 获取脚本所在目录
script_dir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ ! -d "$script_dir/build" ]; then
  mkdir "$script_dir/build"
fi

# 进入build目录
cd "$script_dir/build"

# 根据输入的参数选择操作
if [ $1 = "clean" ]; then
  # 清除构建
  rm -rf *
  echo "构建已清除"
elif [ $1 = "x86" ]; then
  # 进行cmake构建
  if [ -n "$2" ]; then
    cmake -DLOG_LEVEL="$2" ..
  else
    cmake ..
  fi

  # 检查cmake是否成功
  if [ $? -eq 0 ]; then
    # 使用make编译程序
    make

    # 检查make编译是否成功
    if [ $? -eq 0 ]; then
      echo "编译成功"
    else
      echo "make编译失败"
      exit 1
    fi
  else
    echo "cmake构建失败"
    exit 1
  fi
elif [ $1 = "arm" ]; then
  # 使用交叉编译工具链进行编译
  # 确保交叉编译工具链已安装并位于正确的路径下
  export CXX=aarch64-linux-gnu-g++

  # 进行cmake构建
  if [ -n "$2" ]; then
    cmake -DLOG_LEVEL="$2" ..
  else
    cmake ..
  fi

  # 检查cmake是否成功
  if [ $? -eq 0 ]; then
    # 使用make编译程序
    make

    # 检查make编译是否成功
    if [ $? -eq 0 ]; then
      echo "编译成功"
    else
      echo "make编译失败"
      exit 1
    fi
  else
    echo "cmake构建失败"
    exit 1
  fi
else
  echo "参数错误，请输入 x86 或 clean"
  exit 1
fi