#builds jekyll HTML pages and transfers them to 'public' folder
#cd jekyll
bundle exec jekyll build
cp _site/* ../public
