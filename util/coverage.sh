#!/bin/bash

#generate_variables() {
#    echo "generate coverage variables"

    type_defs_global=$(cat */*.c | grep "struct Type_ "| sed 's/static //' | sed 's/extern //' | grep "= {"  | grep -v '@' | cut -d ' ' -f 3 | tr '\n' ':')
    type_name_global=$(cat */*.c | grep "struct Type_ "| sed 's/static //' | sed 's/extern //' | grep "= {"  | grep -v '@' | cut -d '"' -f 2 | tr '\n' ':')

    export type_defs_array=(${type_defs_global//:/ })
    export type_name_array=(${type_name_global//:/ })
#}


#[ -z "$type_defs_array" ] && generate_variables

defs2name() {
	for i in "${!type_defs_array[@]}"
	do
	    [ "$1" = "${type_defs_array[$i]}" ] && echo "${type_name_array[$i]}" && return 0
	done
	return 1
}

op2sign() {
	[ "$1" = "op_assign"       ] && echo "="      && return 0
	[ "$1" = "op_plus"         ] && echo   "+"    && return 0
	[ "$1" = "op_minus"        ] && echo "-"      && return 0
	[ "$1" = "op_times"        ] && echo "*"      && return 0
	[ "$1" = "op_divide"       ] && echo "/"      && return 0
	[ "$1" = "op_percent"      ] && echo "%"      && return 0
	[ "$1" = "op_and"          ] && echo "&&"     && return 0
	[ "$1" = "op_or"           ] && echo  "||"    && return 0
	[ "$1" = "op_eq"           ] && echo  "=="    && return 0
	[ "$1" = "op_neq"          ] && echo "!="     && return 0
	[ "$1" = "op_gt"           ] && echo ">"      && return 0
	[ "$1" = "op_ge"           ] && echo ">="     && return 0
	[ "$1" = "op_lt"           ] && echo "<"      && return 0
	[ "$1" = "op_le"           ] && echo "<="     && return 0
	[ "$1" = "op_shift_left"   ] && echo "<<"     && return 0
	[ "$1" = "op_shift_right"  ] && echo ">>"     && return 0
	[ "$1" = "op_s_or"         ] && echo "|"      && return 0
	[ "$1" = "op_s_and"        ] && echo "&"      && return 0
	[ "$1" = "op_s_xor"        ] && echo "^"      && return 0
	[ "$1" = "op_plusplus"     ] && echo "++"     && return 0
	[ "$1" = "op_minusminus"   ] && echo "--"     && return 0
	[ "$1" = "op_exclamation"  ] && echo "!"      && return 0
	[ "$1" = "op_tilda"        ] && echo "~"      && return 0
	[ "$1" = "op_new"          ] && echo "new"    && return 0
	[ "$1" = "op_spork"        ] && echo "spork"  && return 0
	[ "$1" = "op_typeof"       ] && echo "typeof" && return 0
	[ "$1" = "op_sizeof"       ] && echo "sizeof" && return 0
	[ "$1" = "op_chuck"        ] && echo "=>"     && return 0
	[ "$1" = "op_plus_chuck"   ] && echo "+=>"    && return 0
	[ "$1" = "op_minus_chuck"  ] && echo "-=>"    && return 0
	[ "$1" = "op_times_chuck"  ] && echo "*=>"    && return 0
	[ "$1" = "op_divide_chuck" ] && echo "/=>"    && return 0
	[ "$1" = "op_modulo_chuck" ] && echo "%=>"    && return 0
	[ "$1" = "op_rand"         ] && echo "&&=>"   && return 0
	[ "$1" = "op_ror"          ] && echo "||=>"   && return 0
	[ "$1" = "op_req"          ] && echo "==>"    && return 0
	[ "$1" = "op_rneq"         ] && echo "!=>"    && return 0
	[ "$1" = "op_rgt"          ] && echo ">=>"    && return 0
	[ "$1" = "op_rge"          ] && echo ">==>"   && return 0
	[ "$1" = "op_rlt"          ] && echo "<=>"    && return 0
	[ "$1" = "op_rle"          ] && echo "<==>"   && return 0
	[ "$1" = "op_rsl"          ] && echo "<<=>"   && return 0
	[ "$1" = "op_rsr"          ] && echo ">>=>"   && return 0
	[ "$1" = "op_rsand"        ] && echo "&=>"    && return 0
	[ "$1" = "op_rsor"         ] && echo "|=>"    && return 0
	[ "$1" = "op_rsxor"        ] && echo "^=>"    && return 0
	[ "$1" = "op_unchuck"      ] && echo "=<"     && return 0
	[ "$1" = "op_rinc"         ] && echo "++=>"   && return 0
	[ "$1" = "op_rdec"         ] && echo "--=>"   && return 0
	[ "$1" = "op_runinc"       ] && echo "++=<"   && return 0
	[ "$1" = "op_rundec"       ] && echo "--=<"   && return 0
	[ "$1" = "op_at_chuck"     ] && echo "@=>"    && return 0
	[ "$1" = "op_at_unchuck"   ] && echo "@=<"    && return 0
	[ "$1" = "op_trig"         ] && echo "]=>"    && return 0
	[ "$1" = "op_untrig"       ] && echo "]=<"    && return 0
	return 1
}

type_operator() {
	operator=$(op2sign $(echo "$1" | cut -d ',' -f2))
	left=$(echo "$1" | cut -d ',' -f3 | sed 's/\&//')
	right=$(echo "$1" | cut -d ',' -f4 | sed 's/\&//')
	[ "$left" = " NULL" ] || left=$(defs2name $(echo "$1" | cut -d ',' -f3 | sed 's/\&//'))
	[ "right" = " NULL" ] || right=$(defs2name $(echo "$1" | cut -d ',' -f4 | sed 's/\&//'))
	echo "//testing operator for $left and $right" >> "$2"
	printf "{\n" >> "$2"
	[ "$left"  ] && printf "\t%s\tvariable1;\n" "$left"  >> "$2"
	[ "$right" ] && printf "\t%s\tvariable2;\n" "$right" >> "$2"
	printf "\t<<< " >> "$2"
	[ "$left"  ] && printf "variable1" >> "$2"
	printf "%s" "$operator" >> "$2"
	[ "$right" ] && printf "variable2" >> "$2"
	printf " >>>;\n}\n\n" >> "$2"
}

type_function() {
	arg_list_array=(${arg_list_global//:/ })
	printf "\n\ta.%s(" "$1" >> "$3"
	for i in "${!arg_list_array[@]}"
	do
		printf "%s" "${arg_list_array[$i]}" >> "$3"
		[ -z "${arg_list_array[$i + 1]}" ] || printf ", " >> "$3"
	done
	printf ");\n}\n\n" >> "$3"
}

check_line() {
	return $(echo "$1" | grep "$2" > /dev/null)
}

type_variable() {
	echo "<<<a.$(echo "$1" | cut -d '"' -f4)>>>;" >> "$2"
}

[ "$#" -lt 2 ] && echo "usage: $0 file out_dir." && return 1


while read -r line
do
	# attempts to skip comments
    [ "${line:0:2}" = '/*' ] && continue
    [ "${line:0:2}" = '//' ] && continue

	check_line "$line" "add_global_type"   && {
		type_name=$(defs2name $(echo "$line" | cut -d '&' -f2| cut -d ")" -f1))

		echo -e "// coverage for '$type_name'. (generated by $0)\n" > "$type_name.gw"
		printf "%s a;\n\n" "$type_name" >> "$2/$type_name.gw" && continue
 	}
	check_line "$line" "import_[s|m]var" && type_variable "$line" "$type_name.gw"
	check_line "$line" "new_DL_Func"     && {
		FUNC=$(echo "$line" | grep new_DL_Func |cut -d '"' -f 4)
		printf "// testing \'%s\'\n{\n" $FUNC >> "$2/$type_name.gw"
	}
	check_line "$line" "dl_func_add_arg" && {
		var_type=$(echo "$line" |cut -d '"' -f 2)
		if [ "${var_type: -2}" = "[]" ]
		then
			printf "\t%s" "${var_type:0:${#var_type}-2}" >> "$2/$type_name.gw"
			printf "\t%s[1024]" $(echo "$line" |cut -d '"' -f 4) >> "$2/$type_name.gw"
		else
			printf "\t%s" "$var_type" >> "$2/$type_name.gw"
			printf "\t%s" $(echo "$line" |cut -d '"' -f 4) >> "$2/$type_name.gw"
		fi
		arg_list_global+=$(echo "$line" |cut -d '"' -f 4):
		printf ";\n" >> "$type_name.gw" && continue
	}
	check_line "$line" "import_[s|m]fun" && {
		type_function "$FUNC" "${#arg_list_global[@]}" "$2/$type_name.gw"
		unset FUNC arg_list_global && continue
	}
	check_line "$line" "add_binary_op"   && type_operator "$line" "$2/$type_name.gw"
done < "$1"
