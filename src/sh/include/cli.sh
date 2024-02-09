[ -z "${HEADER_SCRIPTS_SH_CLI_+x}" ] && {
    HEADER_SCRIPTS_SH_CLI_=

    . io.sh

    # example usage:   std_parse_args # std function call
    #
    #                  --opt=a,alpha  # define short and long options. any --func, --var, etc. that come after --opt apply to it.
    #                  --func=my_func # define function for given string to be passed to
    #                                 # if a --var was defined, --var would be set before func is executed regardless of order.
    #                  --type=float,0.0..1.0,my_type_func() # ensure given string is a float in range [0,1]
    #                                                       # also pass string through my_type_func and read exit code.
    #                                                       # for x..y syntax: x XOR y can be omitted but not both,
    #                                                       #                  for float number syntax is same as C
    #                                                       #                  ("0.", ".0", "0", "0.0" are valid, but not "." or others)
    #
    #                  --opt=v,verbose
    #                  --var=verbosity
    #                  --type=cardinal # variable is set to how many of this option was given
    #
    #                  --opt=q,quiet
    #                  --var=verbosity
    #                  --type=-cardinal # subtract the number of times this option was given from verbosity
    #
    #                  --opt=x,exec
    #                  --noop # do nothing. useful for compatibility options.
    #                  --type=-f,755,~myregex,-5..5;$1;-d;$1 # test flags are supported.
    #                                                        # anything matching [0-7]{3} is assumed to be a mode.
    #                                                        # anything that starts with ~ is assumed to be an extended regex.
    #                                                        # file path must be in range [-5,5]. if -5 or 5 was omitted, no min/max limit would exist.
    #                                                        # $1 is replaced with the first column (separated by ;).
    #                                                        # each column applies to the respective argument.
    #                                                        # if less columns are given than argument count, the last column applies to the rest.
    #                                                        # columns cannot be left empty or blank. use same for same as before, or explicitly provide $ with correct index.
    #                                                        # $ indices start at 1.
    #                  --args=..8 # accept 0 to 8 (both inclusive) arguments
    #                  --required # this option is required
    #
    #                  --opt=t,tmp-dir # define option
    #                  --env=TMPDIR # set to $TMPDIR if exists
    #                  --var=opt_tmp_dir # define variable to be set
    #                  --type=-d # ensure given string is a path to a directory that exists
    #                  --args=1 # define argument for defined option.
    #                           # opt_tmp_dir would have been a boolean function if --args was not given or if --args=0 was given. less than 0 is invalid.
    #                           # --tmp-dir has higher priority than $TMPDIR.
    #
    #                  --pos=..1 # load next positional argument into variable. require none. --pos=1 would have required exactly 1.
    #                  --var=first_pos_arg # define new variable
    #
    #                  --pos=1..3 # load next 3 positional arguments into variable, separated by newlines. require only 1.
    #                  --var=next_pos_args # define new variable
    #                  --sep='\n'
    #
    #                  --pos=.. # load all other positional arguments into file, separated by nuls. require none.
    #                  --var=other_pos_args # define new variable
    #                  --file # offload content of variable to file
    #                  --sep='\0'
    #
    #                  -- # end of options. required.
    #                  "$@" # pass arguments


    std_parse_args() {
        __seen_double_dash=0
        while [ "$#" -gt 0 ]; do
            case "$1" in
                --) __seen_double_dash=1; shift; break;;
                --args=*) ;;
                --env=*) ;;
                --file) ;;
                --func=*) ;;
                --noop=*) ;;
                --opt=*) ;;
                --pos=*) ;;
                --required) ;;
                --sep=*) ;;
                --type=*) ;;
                --var=*) ;;
                *) std_logerrq "invalid parsing instruction";;
            esac

            shift
        done

        [ "$__seen_double_dash" -eq 0 ] && std_logerrq "missing end of instructions marker"
        unset __seen_double_dash

        # remaining arguments are actual arguments and not instructions for parsing.

        __print_help() {
            :
        }

        while [ "$#" -gt 0 ]; do
            [ -n "${1##-*}" ] && {
                # TODO: search for matching --pos

                # TODO:
                # if doesn't have matching --pos
                #     die
                # endif

                # TODO: load into matching --pos

                shift
                continue
            }

            if [ "${1%"${1#??}"}" != "--" ]; then
                __sopt="${1#?}"

                while [ -n "$__sopt" ]; do
                    __opt="${__sopt%"${__sopt#?}"}"

                    case "$__opt" in
                        h) __print_help;;
                        *) __print_usage "-$__opt"; exit 0;;
                    esac

                    __sopt="${__sopt#?}"
                done
            else
                __opt="${1#--}"
                case "$__opt" in
                    "") shift; break;;
                    *) __print_usage "--$__opt"; exit 0;;
                esac
            fi

            shift
        done

        for __i; do
            # TODO: search for matching --pos

            # TODO:
            # if doesn't have matching --pos
            #     die
            # endif

            # TODO: load into matching --pos

            :
        done
    }
}
