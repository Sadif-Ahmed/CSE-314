#!/bin/bash



touch $1.sh

chmod 777 $1.sh

echo "#!/bin/bash" > $1.sh

gedit $1.sh & disown


