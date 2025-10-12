#!/bin/bash

# إنشاء مجلد البناء إذا لم يكن موجودًا
mkdir -p build
cd build

# تنظيف البناء السابق
rm -rf *

# تهيئة CMake
cmake ..

# بناء المشروع
make -j$(nproc)

# التحقق من نجاح البناء
if [ $? -eq 0 ]; then
    echo "Build successful! You can run the application with:"
    echo "./AdvancedDownloadManager"
else
    echo "Build failed. Please check the errors above."
fi
