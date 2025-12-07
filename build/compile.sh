#!/bin/bash
set -euo pipefail  # 严格模式：报错立即退出

# ==================== 基础配置（不变）====================
ROOT_DIR="../"
SRC_DIR="${ROOT_DIR}src"
HEAD_DIR="${ROOT_DIR}head"
OBJ_DIR="${ROOT_DIR}out/obj"
BIN_DIR="${ROOT_DIR}out/bin"
TARGET="main"
CONFIG_FILE="${ROOT_DIR}define"

# ==================== 颜色和样式定义（不变）====================
RED="\033[31m"
GREEN="\033[32m"
YELLOW="\033[33m"
BLUE="\033[34m"
NC="\033[0m"

# ==================== 函数定义 ====================
# 中文报错处理
error_exit() {
    echo -e "\n${RED}[错误] $1${NC}"
    exit 1
}

# 交互式选择菜单
show_menu() {
    echo -e "\n${BLUE}=======================================${NC}"
    echo -e "${BLUE}          C++ 编译脚本（交互式）        ${NC}"
    echo -e "${BLUE}=======================================${NC}"
    echo -e "1. 直接编译（使用上次编译的中间文件）"
    echo -e "2. 清理后编译（先删除旧产物，再重新编译）"
    echo -e "3. 仅清理编译产物（不编译）"
    echo -e "${BLUE}=======================================${NC}"
}

# 选择编译模式（RELEASE/DEBUG）
choose_build_mode() {
    echo -e "\n${YELLOW}[选择编译模式]${NC}"
    read -p "是否生成 DEBUG 调试信息？（y=是，n=否，默认n）：" debug_choice
    # 默认不生成 DEBUG 信息
    debug_choice=${debug_choice:-n}
    if [[ "$debug_choice" =~ ^[Yy]$ ]]; then
        DEBUG_MODE=1
        echo -e "${YELLOW}[提示] 已选择 DEBUG 模式（生成调试信息，支持 gdb 调试）${NC}"
    else
        DEBUG_MODE=0
        echo -e "${YELLOW}[提示] 已选择 RELEASE 模式（无调试信息，运行更快、体积更小）${NC}"
    fi
}

# ==================== 步骤1：显示菜单并选择流程 ====================
show_menu
read -p "请选择操作（输入1/2/3，默认1）：" action_choice
# 默认选择「直接编译」
action_choice=${action_choice:-1}

# 解析用户选择
case "$action_choice" in
    1)
        ACTION="compile"
        CLEAN_FIRST=0
        ;;
    2)
        ACTION="compile"
        CLEAN_FIRST=1
        echo -e "${YELLOW}[提示] 即将执行「清理后编译」...${NC}"
        ;;
    3)
        ACTION="clean_only"
        ;;
    *)
        error_exit "无效选择：$action_choice，请输入1/2/3"
        ;;
esac

# ==================== 步骤2：选择编译模式（仅编译时需要）====================
if [ "$ACTION" = "compile" ]; then
    choose_build_mode
fi

# ==================== 步骤3：检查依赖和配置 ====================
echo -e "\n${BLUE}=== 开始初始化配置 ===${NC}"

# 检查g++是否安装
if ! command -v g++ &> /dev/null; then
    error_exit "未找到g++编译器，请先安装：sudo apt install g++（Ubuntu/Debian）或 yum install gcc-c++（CentOS）"
fi

# 检查配置文件是否存在
if [ ! -f "$CONFIG_FILE" ]; then
    error_exit "未找到根目录的配置文件 ${CONFIG_FILE}，请在项目根目录创建define文件"
fi

# 加载配置文件
echo -e "${YELLOW}[提示] 正在加载编译配置...${NC}"
source "$CONFIG_FILE" || error_exit "配置文件 ${CONFIG_FILE} 格式错误（键值对如：OPT_LEVEL=-O2）"

# ==================== 步骤4：动态生成编译参数（根据DEBUG模式）====================
OPT_LEVEL=${OPT_LEVEL:--O2}
LIBS=${LIBS:-""}
# 基础编译参数（无 -g）
DEFAULT_EXTRA_FLAGS="-Wall -Wextra -std=c++17"
if [ "$ACTION" = "compile" ] && [ $DEBUG_MODE -eq 1 ]; then
    DEFAULT_EXTRA_FLAGS="$DEFAULT_EXTRA_FLAGS -g"  # DEBUG模式添加 -g
fi
EXTRA_FLAGS=${EXTRA_FLAGS:-$DEFAULT_EXTRA_FLAGS}
INCLUDE_PATH=${INCLUDE_PATH:--I${ROOT_DIR}head}
LIB_PATH=${LIB_PATH:-""}

# ==================== 步骤5：执行清理操作（如需）====================
if [ "$ACTION" = "clean_only" ] || [ $CLEAN_FIRST -eq 1 ]; then
    echo -e "\n${YELLOW}[提示] 正在清理编译产物...${NC}"
    # 清理目标文件和可执行文件
    rm -rf "${OBJ_DIR}"/*.o
    rm -rf "${BIN_DIR}/${TARGET}"
    # 全清理（仅「仅清理」时删除目录）
    if [ "$ACTION" = "clean_only" ]; then
        rm -rf "${OBJ_DIR}"
        rm -rf "${BIN_DIR}"
        echo -e "${GREEN}[成功] 已全清理所有编译产物${NC}"
        exit 0
    fi
    echo -e "${GREEN}[成功] 已清理旧编译产物${NC}"
fi

# ==================== 步骤6：检查源文件（仅编译时）====================
if [ "$ACTION" = "compile" ]; then
    SRC_FILES=$(find "$SRC_DIR" -type f -name "*.cpp")
    if [ -z "$SRC_FILES" ]; then
        error_exit "在 ${SRC_DIR} 目录下未找到任何.cpp源文件，请检查目录或添加源文件"
    fi

    # 生成目标文件路径
    OBJ_FILES=()
    for src in $SRC_FILES; do
        obj_name=$(basename "$src" .cpp).o
        obj_path="$OBJ_DIR/$obj_name"
        OBJ_FILES+=("$obj_path")
    done
fi

# ==================== 步骤7：创建输出目录（仅编译时）====================
if [ "$ACTION" = "compile" ]; then
    echo -e "\n${YELLOW}[提示] 检查输出目录...${NC}"
    mkdir -p "$OBJ_DIR" "$BIN_DIR" || error_exit "创建目录 ${OBJ_DIR} 或 ${BIN_DIR} 失败，请检查权限"
fi

# ==================== 步骤8：编译源文件（仅编译时）====================
if [ "$ACTION" = "compile" ]; then
    echo -e "\n${BLUE}=== 开始编译源文件（共 $(echo $SRC_FILES | wc -w) 个文件）===${NC}"
    for i in "${!SRC_FILES[@]}"; do
        src=${SRC_FILES[$i]}
        obj=${OBJ_FILES[$i]}
        src_name=$(basename "$src")
        
        echo -e "\n${YELLOW}[编译] 正在处理：$src_name${NC}"
        g++ "$src" -c -o "$obj" \
            $INCLUDE_PATH \
            $OPT_LEVEL \
            $EXTRA_FLAGS \
            2>&1 | sed "s/error:/${RED}错误：${NC}/g; s/warning:/${YELLOW}警告：${NC}/g"
        
        if [ $? -eq 0 ]; then
            echo -e "${GREEN}[成功] $src_name → $(basename "$obj")${NC}"
        else
            error_exit "编译 $src_name 失败，请查看上方错误信息"
        fi
    done
fi

# ==================== 步骤9：链接目标文件（仅编译时）====================
if [ "$ACTION" = "compile" ]; then
    echo -e "\n${BLUE}=== 开始链接目标文件 ===${NC}"
    ELF_PATH="$BIN_DIR/$TARGET"

    # Ensure the same extra flags used for compilation (e.g. -pthread) are
    # also applied at link time. Passing $EXTRA_FLAGS here avoids undefined
    # references like pthread_create when using std::thread or pthreads.
    g++ "${OBJ_FILES[@]}" -o "$ELF_PATH" \
        $EXTRA_FLAGS \
        $LIB_PATH \
        $LIBS \
        2>&1 | sed "s/error:/${RED}错误：${NC}/g; s/warning:/${YELLOW}警告：${NC}/g"

    if [ $? -eq 0 ]; then
        echo -e "${GREEN}[成功] 链接完成！生成可执行文件：${ELF_PATH}${NC}"
    fi
fi

# ==================== 步骤10：编译完成提示（仅编译时）====================
if [ "$ACTION" = "compile" ]; then
    echo -e "\n${GREEN}=== 编译全部完成！===${NC}"
    echo -e "📁 目标文件目录：$(realpath --relative-to=${ROOT_DIR} "$OBJ_DIR")"
    echo -e "🚀 可执行文件：$(realpath --relative-to=${ROOT_DIR} "$ELF_PATH")"
    echo -e "💡 运行命令：cd ${ROOT_DIR} && $(realpath --relative-to=${ROOT_DIR} "$ELF_PATH")"
    if [ $DEBUG_MODE -eq 1 ]; then
        echo -e "🔧 调试命令：cd ${ROOT_DIR} && gdb $(realpath --relative-to=${ROOT_DIR} "$ELF_PATH")"
    fi
fi
