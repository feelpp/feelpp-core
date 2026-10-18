#! /bin/sh
# SPDX-FileCopyrightText: 2025 Université de Strasbourg
# SPDX-FileContributor: Christophe Prud'homme
# SPDX-FileContributor: Vincent Chabannes
# SPDX-License-Identifier: LGPL-3.0-or-later


dir=${1:-.}
for i in $(find $dir -name "*.adoc" | grep pages ); do
    if grep "^:page-jupyter:" $i 1> /dev/null; then
        echo -n "Generating Jupyter Notebook for $i... "
        ipynb=$(echo $i | sed 's/.*\/pages\///' )
        dir=$(dirname $ipynb)
        mkdir -p jupyter/$dir
        asciidoctor -r asciidoctor-jupyter -b jupyter $i -o jupyter/$dir/$(basename $ipynb .adoc).ipynb
        echo "jupyter/$dir/$(basename $ipynb .adoc).ipynb created"
    fi;
done
