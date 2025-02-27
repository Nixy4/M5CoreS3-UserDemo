cmake_file=./CMakeLists.txt
windir=/mnt/d/ESP/flash_download_tool/temp

target=$(grep -E "^project\(" "$cmake_file" | sed -E 's/project\(([^)]+)\)/\1/')
target=$(echo $target | tr -d '\r')
builddir=./build
bootloader=${builddir}/bootloader/bootloader.bin # 0x0000
partition=${builddir}/partition_table/partition-table.bin # 0x8000
ota_data=${builddir}/ota_data_initial.bin # 0xD000
model=${builddir}/srmodels/srmodels.bin # 0x10000
main=${builddir}/${target}.bin # 0x410000
storage=${builddir}/storage.bin # 0xC10000

rm -f ${windir}/**.bin
echo 目标文件夹: ${windir} 已清空

if [ -f $bootloader ]; then
  echo 已复制 bootloader
  cp $bootloader ${windir}/bootloader.bin
fi

if [ -f $partition ]; then
  echo 已复制 partition-table
  cp $partition ${windir}/partition-table.bin
fi

if [ -f $ota_data ]; then
  echo 已复制 ota_data
  cp $ota_data ${windir}/ota_data_initial.bin
fi

if [ -f $model ]; then
  echo 已复制 model
  cp $model ${windir}/srmodels.bin
fi

if [ -f $main ]; then
  echo 已复制 main
  cp $main ${windir}/main.bin
fi

if [ -f $storage ]; then
  echo 已复制 storage
  cp $storage ${windir}/storage.bin
fi

