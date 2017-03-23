#!/bin/sh
# deploy site on gh-pages

function post()
{
	if [ ls -d */ | grep "$1" ]
	then
		echo "first arg must be a directory, or 'run' or 'deploy'"
		return
	fi
	echo -e "---\nlayout:     page\ntitle:      $2" > $1/$2.html
	echo -e "categories: $1\n---\n" >> $1/$2.html
	nano $1/$2.html
}

function deploy()
{
#	git add .
#	git commit -am 'Pre-deploy commit :smile:'
#	git push
	bundle exec jekyll build --baseurl https://fennecdjay.github.io/Gwion
	mv _site /tmp
	rm .jekyll-metadata
	git checkout gh-pages || { echo "error: gh-pages checkout"; return; }
	rm -rf *
	mv /tmp/_site/* .
	git add .
	git commit -am 'Yeah. Built from subdir'
	git push
	rm .jekyll-metadata
	git checkout site || { echo "error: site checkout"; return; }
}

on_int()
{
	sed -i '/base/s/^#//g' _config.yml #uncomment
	rm -rf _site
}
#run the size locally. access with localhost:4000
function run()
{
	trap on_int INT
	sed -i '/base/s/^/#/g' _config.yml #comment
	bundle exec jekyll s
}

# remove _site if interupted or ...
trap 'rm -rf _site; exit' INT
trap 'rm -rf _site; exit' QUIT
trap 'rm -rf _site; exit' TERM
trap 'rm -rf _site; exit' EXIT

function send_gist()
{
	echo $(gist -u $1 -f $2| sed "s/https:\/\/gist.github.com\///") >> known_gist
}

function doc()
{
#	mkdir Gwion
	rm -rf doc
#	for dir in core eval drvr lang include
#	do  git checkout master -- "$a"
#	done
	doxygen

for a in doc/*.html
do
#        sed -i 's/<script type="text\/javascript" src="/<script type="text\/javascript" src="doc\//' $a;
		sed -i 's/{%/{ %/' $a
		content=$(cat $a)
        echo -e "---\nlayout: homepage\ntitle: $(echo ${a/.html//} | sed 's#doc/##')\nimage:\n  feature: abstract-1.jpg\n---\n" > $a
        echo "$content" >> $a
done
	sed -i 's/,url:"/,url:"doc\//' doc/menudata.js
	rm -rf search
	cp -r doc/search search
#	mv doc/search search

#	cp search.js doc/search

#	sed -i "s/this.resultsPath + '/doc\/' + this.resultsPath + '/" doc/search/search.js
#for a in doc/search/*.js
#do
#	[ "$a" = "doc/search/searchdata.js" ] && continue
#	[ "$a" = "doc/search/search.js" ] && continue
#	sed -i "s#'\],\['../#'\],\['../doc/#g" $a
#	sed -i "s#',\['../#'\],\['../doc/#g" $a
#done

}

function examples()
{
	rm -rf examples Gwion-examples
	git checkout master -- examples
	mv examples Gwion-examples
	mkdir examples
	for ex in $(ls Gwion-examples/*.gw)
	do
		NAME=$(basename ${ex/.gw//})
		echo -e "---\nlayout: default\ntitle: example $NAME\ncategories: [examples]\nimage:\n  feature: abstract-1.jpg\n#  credit:\n#  creditlink:\\n---\n<br>this page documents <b>$NAME.gw</b><br><p>" > examples/$NAME.html
		pygmentize -f html $ex >> examples/$NAME.html
	echo "</p>" >> examples/$NAME.html
	done
	echo '---
layout: homepage
title:	{{post.title}}
---
  <h1>Examples</h1>
  <ul class="posts">
{% for post in site.pages %}
	{% if post.categories == "examples" %}
		<li><span><a href="{{ site.baseurl }}{{ post.url }}">{{post.title}}</a></li>
	{% endif %}
{% endfor %}
  </ul>' > examples/index.html


	rm -r Gwion-examples
	echo "put example content, whith highligting"
	echo "you should be done."
}

function code()
{
	rm -f /tmp/file.gw
	nano /tmp/file.gw
	pygmentize -f html /tmp/file.gw | xclip -i
	echo "should be"
	pygmentize /tmp/file.gw
	rm -f /tmp/file.gw
}

# parse arguments
if [ "$#" -lt 1 ]
then
	echo "usage: $0 run/send_gist \$2 \$3/deploy/code/examples/post $1 $2/"
elif [ "${1}" = 'run' ]
then
	run
elif [ "${1}" = 'gist' ]
then
	send_gist $2 $3
elif [ "${1}" = 'doc' ]
then
	doc
elif [ "${1}" = 'deploy' ]
then
	deploy
elif [ "${1}" = 'code' ]
then
	code
elif [ "${1}" = 'examples' ]
then
	examples
else
	post $1 $2
fi
