for file in *.cxx
do
  pygmentize -f latex $file > ${file%%cxx}tex
done

for file in *.h
do
  pygmentize -f latex $file > ${file%%h}tex
done
