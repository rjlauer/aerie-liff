for file in *.cxx
do
  pygmentize -f latex $file > ${file%%cxx}tex
done

for file in *.py
do
  pygmentize -f latex $file > ${file%%py}tex
done
