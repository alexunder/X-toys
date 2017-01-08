LNX=$HOME/mohism/company/CellMatrix/kernel/tegra
cd /
find  $LNX                                                                \
          -path "$LNX/arch/*" ! -path "$LNX/arch/arm*" -prune -o               \
          -path "$LNX/tmp*" -prune -o                                           \
          -path "$LNX/Documentation*" -prune -o                                 \
          -path "$LNX/scripts*" -prune -o                                       \
          -name "*.[chxsS]" -print > $HOME/mohism/company/CellMatrix/kernel/tegra/cscope.files

cd $LNX
cscope -b -q -k
