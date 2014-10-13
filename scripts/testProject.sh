#!/bin/bash
LC_ALL=C # posix

#-------
# Verze 1.0   - scitani, odcitani, nasobeni, deleni a jejich kombinace - TESTS 01-05
#				Pouze pro programy bez vstupu z stdin
# Verze 2.0   - Upraveny veskere vypisy, sak zkus to sam
#				STALE pouze pro programy bez vstupu z stdin
# Verze 2.1   - Upravy vypisu
#				PODPORUJE programy se vstupem (vstup zapsan v souboru nazev_programu.input)
# Verze 3.0   - Dela statistiky uspechu/neuspechu a uklada je do souboru "vysledkyTestu.txt"
#				s ukladanim na zacatek, aby se dal pouzit prikaz "head", ktery vypise
#				prvnich 10 radku. Je to vsak narocnejsi pro disk. Pokud by to nekomu vadilo,
#				sem tam smazte "vysledkyTestu.txt"
#				Bohuzel pro ucel statistiky musi vytvaret pomocne soubory do slozky ./tmp,
#				kterou pak zase uvolni. Dale predpoklada neexistenci teto pred
#				spustenim skriptu.
# Verze 3.1   - Na popud Petra upraven vypis pridanim dalsih hodnot
#				Zjistil jsem, ze tak pozde v noci mi to nemysli a neumim zaklady algebry, 
#				nove testy proto budou az zitra
#-------
print_help()
{ # tisk napovedy
	echo
	echo "Verze 3.0 - statistiky do souboru vysledkyTestu.txt, pak head vysledkyTestu.txt"
	echo "RADA: pokud je vypisu moc, zvol mene testovacich vstupu (podadresar TESTu) napr: ./testProject ./project ./TESTS/01aritmeticOperation/01scitani"
	echo "-d  Mazani souboru .outputProject a jine: maze vsechny vytvorene soubory timto skriptem. Priklad pouziti: ./testProject -d ./project ./TESTS/ - toto delej pred _git push_ !!!!"
	echo "-h  Vypis teto napovedy: ./testProject -h"
	echo "Pro soubory .fal s pripadnym vstupem z stdin v nazev_programu.input"
    echo "Skript preklada vstupni program napsany v jazyce IFJ12 a porovnava jeho vystupy s referencnimi, porovnava navratove kody s referencimi"
    echo "Prvni parametr = cesta k prelozenemu projektu"
	echo "Druhy parametr = cesta k adresari TESTS s testy"
        echo "Treti parametr = (volitelny) muze nabyvat hodnoty -v pro klasicke spusteni s valgrindem, nebo hodnoty -v=f pro spusteni s valgrindem s parametry --tool=memcheck --leak-check=yes --show-reachable=yes"
	echo "----------"
	echo "Spusteni skriptu:" 
	echo "napr: ./testProject.sh ./projekt ./TESTS"
	echo "----------"
	echo "Pokud je nalezena odlisnost, dojde k vypisu na stdout a v patricnem souboru je pomoci utility diff vypsan rozdil vypisu naseho projektu s referencim resenim"
	echo "Samozrejme se predpoklada vypnuti vypisu debugovych informaci v souboru ifj_lib.h"
	echo "----------"
	echo "Co znamenaji jednotlive soubory: (vezmeme si priklad program 01.fal)"
	echo "01.fal				- program v jazyce IFJ12"
	echo "01.output				- referencni reseni"
	echo "01.outputProject		- vystup generovany nasim projektem"
	echo "01.diff				- pokud se 01.output a 01.outputProject lisi, zde jsou rozdily"
	echo "01.retcode			- referencni navratovy kod"
	echo "01.retcodeProject		- navratovy kod generovany nasim projektem"
	echo "01.diffRetcode		- pokud se 01.retcode a 01.retcodeProject lisi, zde jsou rozdily"
	echo "Toto te nemusi zajimat, ale nemaz mi to:"
	echo "01.notDoOutput		- slouzi pri generaci 01.output, kdyz merlin generuje spatne vysledky"
	echo "02.notDoRetcode		- slouzi pri generaci 01.retcode, kdyz je zapsan nevalidni program v IFJ12"
	echo "---------"
	echo "Kdyby jsi neco nechapal, klidne pis na FB"
}



deleteALL()
{
# smaze soubory s koncovkou .outputProject
find $2 -name "*.outputProject" | sort -u |  while read file
    do
		rm $file
    done

# smaze soubory s koncovkou .diff
find $2 -name "*.diff" | sort -u |  while read file
    do
		rm $file
    done

# smaze soubory s koncovkou .diffRetcode
find $2 -name "*.diffRetcode" | sort -u |  while read file
    do
		rm $file
    done

# smaze soubory s koncovkou .retcodeProject
find $2 -name "*.retcodeProject" | sort -u |  while read file
    do
		rm $file
    done

}

initCounters()
{
	backupPWD=$PWD
	if [ -d ./tmp ]; then
		# smazu vsechny citace
		echo "Maly problem - slozka ./tmp existuje, smazte ji"
		exit 1
		find ./tmp -name "*.counter" | while read file
		do
			rm $file
		done	
		cd ./tmp
	else
		mkdir ./tmp
		cd ./tmp
	fi
	# zalozeni citacu
	#touch OutputOK.counter RetcodeOK.counter OutputERR.counter RetcodeERR.counter
	echo -n "0" > OutputOK.counter
	echo -n "0" > OutputERR.counter
	echo -n "0" > RetcodeOK.counter
	echo -n "0" > RetcodeERR.counter
	cd $backupPWD
}

disposeCounters()
{
	if [ -d ./tmp ]; then
		# smazu vsechny citace
		find ./tmp -name "*.counter" | while read file
		do
			rm $file
		done	
		rmdir ./tmp
	fi
}

counterOutputOK()
{
	backupPWD=$PWD
	cd ./tmp
	counter=`cat OutputOK.counter`
	counter=$(( $counter + 1 ))
	echo -n $counter > OutputOK.counter
	cd $backupPWD
}

counterRetcodeOK()
{
	backupPWD=$PWD
	cd ./tmp
	counter=`cat RetcodeOK.counter`
	counter=$(( $counter + 1 ))
	echo -n $counter > RetcodeOK.counter
	cd $backupPWD
}

counterOutputERR()
{
	backupPWD=$PWD
	cd ./tmp
	counter=`cat OutputERR.counter`
	counter=$(( $counter + 1 ))
	echo -n $counter > OutputERR.counter
	cd $backupPWD
}

counterRetcodeERR()
{
	backupPWD=$PWD
	cd ./tmp
	counter=`cat RetcodeERR.counter`
	counter=$(( $counter + 1 ))
	echo -n $counter > RetcodeERR.counter
	cd $backupPWD
}


#/////////////////////// HLAVNI CAST SKRIPTU ///////////////////
#///////////////////////////////////////////////////////////////

#// zpracovani parametru napovedy
while getopts '[dh]' opt; do
    case "$opt" in
		d)	# smaze soubory s priponou .outputProject a .diff
			deleteALL
			exit 0
			;;
        h)	# tiskne napovedu
			print_help
			exit 1
            ;;
    esac
done


# kontrola parametru - musi byt zadany dva parametry - viz napoveda
if [ $# -ne 2 -a $# -ne 3 ]; then
	echo "ERR: Nezadan patricny pocet parametru" >&2
	exit 1
fi 

# nastavovani sousteni s valgrindem nebo bez
if [ $# -eq 3 ]; then
    if [ $3 = "-v" ]; then
        echo "Testuji s valgrindem"
        valgrind="valgrind "
    elif [ $3 = "-v=f" ]; then
        echo "Testuji s valgrindem ultra"
        valgrind="valgrind --tool=memcheck --leak-check=yes --show-reachable=yes "
    else
        echo "Chyba v tretim parametru!"
        exit 1
    fi
else
    valgrind=""
fi

deleteALL		# uklid
initCounters	# inicializace citacu pro statistiky

# $1 = soubor obsahujici definice vestavenych funkci, ktere jsou soucasti jazyka IFJ12, ale
#	chybi v jazyce FALCON
# $2 = adresar, ve kterem bude probihat generace vystupnich souboru .output ze vstupnich .fal

# dirWithFalconFiles = cesta adresare, ktery obsahuje soubor(y) s priponou .fal
# FalconFile = cesta souboru s priponou .fal
find $2 -name "*.fal" | sed -e 's/\/[^\/]*$//' | sort -u |  while read dirWithFalconFiles
	do
		echo -e "\e[1mKontrola slozky:\e[0m $dirWithFalconFiles" 
		find $dirWithFalconFiles -name "*fal" | sort -u |  while read FalconFile
		do
			# odstrani priponu .fal
			nameFileFalcon=`echo "$FalconFile" | sed -e 's/^.*\/\(.*\)\.fal$/\1/'`

            #echo "FalconFile = $FalconFile"
            #echo "nameFileFalcon = $nameFileFalcon"
			#falcon -p $1 $FalconFile > $dirWithFalconFiles/$nameFileFalcon.output

			# spusti nas projekt a vygeneruje vystup do souboru s priponou .outputProject
			if [ -f $dirWithFalconFiles/$nameFileFalcon.input ]; then
				# pokud existuje soubor se vstupnimi informacemi, pouzij ho                                                                
				$valgrind $1 $FalconFile > $dirWithFalconFiles/$nameFileFalcon.outputProject < $dirWithFalconFiles/$nameFileFalcon.input
			else			
				# neexistuje soubor se vstupmini informacemi
				$valgrind $1 $FalconFile > $dirWithFalconFiles/$nameFileFalcon.outputProject
			fi

			# navratovy kod naseho projektu ulozi do souboru s priponou .retcodeProject
			echo $? > $dirWithFalconFiles/$nameFileFalcon.retcodeProject

			# porovna navratovy kod naseho projektu s referencnim
			diff $dirWithFalconFiles/$nameFileFalcon.retcode $dirWithFalconFiles/$nameFileFalcon.retcodeProject > /dev/null 2> /dev/null
			diffRetCode=$?

			# kontrola navratoveho kodu
			if [ $diffRetCode -eq 0 ]; then
				# navratovy kod OK
				echo -e "$FalconFile: \e[00;32mOK_retcode\e[00m"
				counterRetcodeOK
			elif [ $diffRetCode -eq 1 ]; then
				# navratovy kod SE LISI
				diff $dirWithFalconFiles/$nameFileFalcon.retcode $dirWithFalconFiles/$nameFileFalcon.retcodeProject > $dirWithFalconFiles/$nameFileFalcon.diffRetcode
				echo -e "$dirWithFalconFiles/$nameFileFalcon.fal: \e[00;31mERR_retcode\e[00m" "ocekavan:" `cat  $dirWithFalconFiles/$nameFileFalcon.retcode` "generovan:" `cat  $dirWithFalconFiles/$nameFileFalcon.retcodeProject`
				counterRetcodeERR
			elif [ $diffRetCode -eq 2 ]; then
				# referencni navratovy kod NEEXISTUJE
				echo -e "$dirWithFalconFiles/$nameFileFalcon.fal: \e[00;31m.retcode neexistuje - VYTVOR HO\e[00m"
			else
				# interni chyba skriptu
				echo -e "\e[00;31mInteralERR:\e[00m $dirWithFalconFiles/$nameFileFalcon.fal: \e[00;31mInterni chyba - kontaktuj autora\e[00m"
			fi

			#if [ $diff1 -ne 0 ]; then
			#diff $dirWithFalconFiles/$nameFileFalcon.retcode $dirWithFalconFiles/$nameFileFalcon.retcodeProject > $dirWithFalconFiles/$nameFileFalcon.diffRetcode
				#echo -e "\e[00;31mERR:\e[00m $dirWithFalconFiles/$nameFileFalcon - \e[00;31mLisi se NAVRATOVY KOD\e[00m"
			#else
				#echo -e "$FalconFile: \e[00;32mOK_retcode\e[00m"
			#fi


			# porovna vystup s referencnim
			diff $dirWithFalconFiles/$nameFileFalcon.output $dirWithFalconFiles/$nameFileFalcon.outputProject > /dev/null 2> /dev/null
			diffOutput=$?

			# kontrola vystupu
			if [ $diffOutput -eq 0 ]; then
				# vystup OK
				echo -e "$FalconFile: \e[01;32mOK_output\e[00m"
				counterOutputOK
			elif [ $diffOutput -eq 1 ]; then
				# vystup SE LISI
				diff $dirWithFalconFiles/$nameFileFalcon.output $dirWithFalconFiles/$nameFileFalcon.outputProject > $dirWithFalconFiles/$nameFileFalcon.diff
				echo -e "$dirWithFalconFiles/$nameFileFalcon.fal: \e[00;31mERR_output\e[00m"
				counterOutputERR
			elif [ $diffRetCode -eq 2 ]; then
				# referencni vystup NEEXISTUJE
				echo -e "$dirWithFalconFiles/$nameFileFalcon.fal: \e[00;31m.output neexistuje - VYTVOR HO\e[00m"
			else
				# interni chyba skriptu
				echo -e "\e[00;31mInteralERR:\e[00m $dirWithFalconFiles/$nameFileFalcon: \e[00;31mInterni chyba - kontaktuj autora\e[00m"
			fi


			# pokud se nerovnaji, vypis nerovnosti do souboru s priponou .diff
			#if [ $? -ne 0 ]; then
			#diff $dirWithFalconFiles/$nameFileFalcon.output $dirWithFalconFiles/$nameFileFalcon.outputProject > $dirWithFalconFiles/$nameFileFalcon.diff
			#	echo -e "\e[00;31mERR:\e[00m $dirWithFalconFiles/$nameFileFalcon - \e[00;31mLisi se REFERENCNI VYSTUP\e[00m"
			#else
				#echo -e "$FalconFile: \e[01;32mOK_outputProject\e[00m"
			#fi
			
		done
		# odradkovani
		echo
	done

# uprava datumu
datum=`echo $(date) | sed -e 's/^\(.*\) \([^ ]*\) \([^ ]*\)$/\1/'`

# celkovy pocet testu Output a Retcode
numberOfAllTestsOutput=`cat ./tmp/OutputOK.counter`
numberOfAllTestsOutput=$(( $numberOfAllTestsOutput + `cat ./tmp/OutputERR.counter` ))

numberOfAllTestsRetcode=`cat ./tmp/RetcodeOK.counter` 
numberOfAllTestsRetcode=$(( $numberOfAllTestsRetcode + `cat ./tmp/RetcodeERR.counter` ))

# vypocet procent
OutputOKpercent=$(( `cat ./tmp/OutputOK.counter` * 100 ))
OutputOKpercent=$(( $OutputOKpercent / $numberOfAllTestsOutput ))

OutputERRpercent=$(( `cat ./tmp/OutputERR.counter` * 100 ))
OutputERRpercent=$(( $OutputERRpercent / $numberOfAllTestsOutput ))

RetcodeOKpercent=$(( `cat ./tmp/RetcodeOK.counter` * 100 ))
RetcodeOKpercent=$(( $RetcodeOKpercent / $numberOfAllTestsRetcode ))

RetcodeERRpercent=$(( `cat ./tmp/RetcodeERR.counter` * 100 ))
RetcodeERRpercent=$(( $RetcodeERRpercent / $numberOfAllTestsRetcode ))

# zaloha dosavadnich vysledku testuu
if [ -f vysledkyTestu.txt ]; then
	# pokud je, presunu
	mv vysledkyTestu.txt ./tmp/vysledkyTestu.old
else
	# pokud jeste nebyl proveden zadny test
	touch ./tmp/vysledkyTestu.old
fi

if [ -d .git ]; then
	GIT_COMMIT=$(git rev-parse HEAD | cut -b '-7')
else
	GIT_COMMIT=""
fi

# prilozeni do souboru
echo -e "[$GIT_COMMIT] [$datum] [OutOK: `cat ./tmp/OutputOK.counter`/$numberOfAllTestsOutput ($OutputOKpercent%) | OutERR: `cat ./tmp/OutputERR.counter`/$numberOfAllTestsOutput ($OutputERRpercent%) | RetOK: `cat ./tmp/RetcodeOK.counter`/$numberOfAllTestsRetcode ($RetcodeOKpercent%) | RetERR: `cat ./tmp/RetcodeERR.counter`/$numberOfAllTestsRetcode ($RetcodeERRpercent%)] [$2]" >> vysledkyTestu.txt

#echo -e "[$datum] [OutOK: `cat ./tmp/OutputOK.counter` OutERR: `cat ./tmp/OutputERR.counter` RetOK: `cat ./tmp/RetcodeOK.counter` RetERR: `cat ./tmp/RetcodeERR.counter`] [$2]" >> vysledkyTestu.txt


# na konec zapiseme zalohu
cat ./tmp/vysledkyTestu.old >> vysledkyTestu.txt
# smazeme zalohu
rm ./tmp/vysledkyTestu.old

# vypis na stdout

#echo -e "[$datum] [OutOK: `cat ./tmp/OutputOK.counter`/$numberOfAllTestsOutput ($OutputOKpercent%) | OutERR: `cat ./tmp/OutputERR.counter`/$numberOfAllTestsOutput ($OutputERRpercent%) | RetOK: `cat ./tmp/RetcodeOK.counter`/$numberOfAllTestsRetcode ($RetcodeOKpercent%) | RetERR: `cat ./tmp/RetcodeERR.counter`/$numberOfAllTestsRetcode ($RetcodeERRpercent%)] [$2]"
#echo -e "[$datum] [OutOK: `cat ./tmp/OutputOK.counter` OutERR: `cat ./tmp/OutputERR.counter` RetOK: `cat ./tmp/RetcodeOK.counter` RetERR: `cat ./tmp/RetcodeERR.counter`] [$2]"
# vypis poslednich deseti hodnot
head vysledkyTestu.txt
disposeCounters # zruseni citacu
echo
exit 0
