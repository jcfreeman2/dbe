echo "refreshing ../../CREDITS.html and ../../CHANGES.html"
rst2html.py ../../CREDITS.txt ../CREDITS.html
rst2html.py ../../CHANGES.txt ../CHANGES.html
rst2html.py ../../RELEASE_CHANGES.txt ../RELEASE_CHANGES.html
echo "building HTML documentation with Sphinx"
echo "1st pass for 'cleaning'"
echo
make html
echo
echo "*******"
echo
echo "2nd pass for real 'building'"
echo
make html
echo "removing old HTML docs from Ric.'s public web folder"
rm -rf $HOME/www/public_afs_web/dbe_help/
echo "copying new HTML docs to Ric.'s public web folder"
cp -r _build/html/ $HOME/www/public_afs_web/dbe_help

