// Generated by Bisonc++ V4.10.01 on Thu, 08 Dec 2016 09:07:01 +0100

// $insert class.ih
#include "parser.ih"

// The FIRST element of SR arrays shown below uses `d_type', defining the
// state's type, and `d_lastIdx' containing the last element's index. If
// d_lastIdx contains the REQ_TOKEN bitflag (see below) then the state needs
// a token: if in this state d_token__ is _UNDETERMINED_, nextToken() will be
// called

// The LAST element of SR arrays uses `d_token' containing the last retrieved
// token to speed up the (linear) seach.  Except for the first element of SR
// arrays, the field `d_action' is used to determine what to do next. If
// positive, it represents the next state (used with SHIFT); if zero, it
// indicates `ACCEPT', if negative, -d_action represents the number of the
// rule to reduce to.

// `lookup()' tries to find d_token__ in the current SR array. If it fails, and
// there is no default reduction UNEXPECTED_TOKEN__ is thrown, which is then
// caught by the error-recovery function.

// The error-recovery function will pop elements off the stack until a state
// having bit flag ERR_ITEM is found. This state has a transition on _error_
// which is applied. In this _error_ state, while the current token is not a
// proper continuation, new tokens are obtained by nextToken(). If such a
// token is found, error recovery is successful and the token is
// handled according to the error state's SR table and parsing continues.
// During error recovery semantic actions are ignored.

// A state flagged with the DEF_RED flag will perform a default
// reduction if no other continuations are available for the current token.

// The ACCEPT STATE never shows a default reduction: when it is reached the
// parser returns ACCEPT(). During the grammar
// analysis phase a default reduction may have been defined, but it is
// removed during the state-definition phase.

// So:
//      s_x[] = 
//      {
//                  [_field_1_]         [_field_2_]
//
// First element:   {state-type,        idx of last element},
// Other elements:  {required token,    action to perform},
//                                      ( < 0: reduce, 
//                                          0: ACCEPT,
//                                        > 0: next state)
// Last element:    {set to d_token__,    action to perform}
//      }

// When the --thread-safe option is specified, all static data are defined as
// const. If --thread-safe is not provided, the state-tables are not defined
// as const, since the lookup() function below will modify them


namespace // anonymous
{
    char const author[] = "Frank B. Brokken (f.b.brokken@rug.nl)";

    enum 
    {
        STACK_EXPANSION = 5     // size to expand the state-stack with when
                                // full
    };

    enum ReservedTokens
    {
        PARSE_ACCEPT     = 0,   // `ACCEPT' TRANSITION
        _UNDETERMINED_   = -2,
        _EOF_            = -1,
        _error_          = 256
    };
    enum StateType       // modify statetype/data.cc when this enum changes
    {
        NORMAL,
        ERR_ITEM,
        REQ_TOKEN,
        ERR_REQ,    // ERR_ITEM | REQ_TOKEN
        DEF_RED,    // state having default reduction
        ERR_DEF,    // ERR_ITEM | DEF_RED
        REQ_DEF,    // REQ_TOKEN | DEF_RED
        ERR_REQ_DEF // ERR_ITEM | REQ_TOKEN | DEF_RED
    };    
    struct PI__     // Production Info
    {
        size_t d_nonTerm; // identification number of this production's
                            // non-terminal 
        size_t d_size;    // number of elements in this production 
    };

    struct SR__     // Shift Reduce info, see its description above
    {
        union
        {
            int _field_1_;      // initializer, allowing initializations 
                                // of the SR s_[] arrays
            int d_type;
            int d_token;
        };
        union
        {
            int _field_2_;

            int d_lastIdx;          // if negative, the state uses SHIFT
            int d_action;           // may be negative (reduce), 
                                    // postive (shift), or 0 (accept)
            size_t d_errorState;    // used with Error states
        };
    };

    // $insert staticdata
    
// Productions Info Records:
PI__ const s_productionInfo[] = 
{
     {0, 0}, // not used: reduction values are negative
     {268, 1}, // 1: expression ->  scalar
     {268, 1}, // 2: expression ->  sequence
     {268, 1}, // 3: expression ->  tuple
     {269, 1}, // 4: scalar (BOOLEAN) ->  BOOLEAN
     {269, 1}, // 5: scalar (INTEGER) ->  INTEGER
     {269, 1}, // 6: scalar (REAL) ->  REAL
     {269, 1}, // 7: scalar (STRING) ->  STRING
     {269, 1}, // 8: scalar (BINARY) ->  BINARY
     {270, 0}, // 9: opt_series ->  <empty>
     {270, 1}, // 10: opt_series ->  series
     {271, 1}, // 11: series ->  scalar
     {271, 3}, // 12: series (',') ->  series ',' scalar
     {272, 3}, // 13: sequence ('[') ->  '[' opt_series ']'
     {273, 3}, // 14: tuple ('{') ->  '{' opt_series '}'
     {274, 1}, // 15: expression_$ ->  expression
};

// State info and SR__ transitions for each state.


SR__ s_0[] =
{
    { { REQ_TOKEN}, { 12} },              
    { {       268}, {  1} }, // expression
    { {       269}, {  2} }, // scalar    
    { {       272}, {  3} }, // sequence  
    { {       273}, {  4} }, // tuple     
    { {       257}, {  5} }, // BOOLEAN   
    { {       258}, {  6} }, // INTEGER   
    { {       259}, {  7} }, // REAL      
    { {       260}, {  8} }, // STRING    
    { {       261}, {  9} }, // BINARY    
    { {        91}, { 10} }, // '['       
    { {       123}, { 11} }, // '{'       
    { {         0}, {  0} },              
};

SR__ s_1[] =
{
    { { REQ_TOKEN}, {            2} }, 
    { {     _EOF_}, { PARSE_ACCEPT} }, 
    { {         0}, {            0} }, 
};

SR__ s_2[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -1} }, 
};

SR__ s_3[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -2} }, 
};

SR__ s_4[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -3} }, 
};

SR__ s_5[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -4} }, 
};

SR__ s_6[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -5} }, 
};

SR__ s_7[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -6} }, 
};

SR__ s_8[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -7} }, 
};

SR__ s_9[] =
{
    { { DEF_RED}, {  1} }, 
    { {       0}, { -8} }, 
};

SR__ s_10[] =
{
    { { REQ_DEF}, {  9} },              
    { {     270}, { 12} }, // opt_series
    { {     271}, { 13} }, // series    
    { {     269}, { 14} }, // scalar    
    { {     257}, {  5} }, // BOOLEAN   
    { {     258}, {  6} }, // INTEGER   
    { {     259}, {  7} }, // REAL      
    { {     260}, {  8} }, // STRING    
    { {     261}, {  9} }, // BINARY    
    { {       0}, { -9} },              
};

SR__ s_11[] =
{
    { { REQ_DEF}, {  9} },              
    { {     270}, { 15} }, // opt_series
    { {     271}, { 13} }, // series    
    { {     269}, { 14} }, // scalar    
    { {     257}, {  5} }, // BOOLEAN   
    { {     258}, {  6} }, // INTEGER   
    { {     259}, {  7} }, // REAL      
    { {     260}, {  8} }, // STRING    
    { {     261}, {  9} }, // BINARY    
    { {       0}, { -9} },              
};

SR__ s_12[] =
{
    { { REQ_TOKEN}, {  2} },       
    { {        93}, { 16} }, // ']'
    { {         0}, {  0} },       
};

SR__ s_13[] =
{
    { { REQ_DEF}, {   2} },       
    { {      44}, {  17} }, // ','
    { {       0}, { -10} },       
};

SR__ s_14[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -11} }, 
};

SR__ s_15[] =
{
    { { REQ_TOKEN}, {  2} },       
    { {       125}, { 18} }, // '}'
    { {         0}, {  0} },       
};

SR__ s_16[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -13} }, 
};

SR__ s_17[] =
{
    { { REQ_TOKEN}, {  7} },           
    { {       269}, { 19} }, // scalar 
    { {       257}, {  5} }, // BOOLEAN
    { {       258}, {  6} }, // INTEGER
    { {       259}, {  7} }, // REAL   
    { {       260}, {  8} }, // STRING 
    { {       261}, {  9} }, // BINARY 
    { {         0}, {  0} },           
};

SR__ s_18[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -14} }, 
};

SR__ s_19[] =
{
    { { DEF_RED}, {   1} }, 
    { {       0}, { -12} }, 
};


// State array:
SR__ *s_state[] =
{
  s_0,  s_1,  s_2,  s_3,  s_4,  s_5,  s_6,  s_7,  s_8,  s_9,
  s_10,  s_11,  s_12,  s_13,  s_14,  s_15,  s_16,  s_17,  s_18,  s_19,
};

} // anonymous namespace ends


// $insert namespace-open
namespace decof
{

namespace cli
{

// If the parsing function call uses arguments, then provide an overloaded
// function.  The code below doesn't rely on parameters, so no arguments are
// required.  Furthermore, parse uses a function try block to allow us to do
// ACCEPT and ABORT from anywhere, even from within members called by actions,
// simply throwing the appropriate exceptions.

parserBase::parserBase()
:
    d_stackIdx__(-1),
    // $insert debuginit
    d_debug__(false),
    d_nErrors__(0),
    // $insert requiredtokens
    d_requiredTokens__(0),
    d_acceptedTokens__(d_requiredTokens__),
    d_token__(_UNDETERMINED_),
    d_nextToken__(_UNDETERMINED_)
{}


void parser::print__()
{
// $insert print
}

void parserBase::clearin()
{
    d_token__ = d_nextToken__ = _UNDETERMINED_;
}

void parserBase::push__(size_t state)
{
    if (static_cast<size_t>(d_stackIdx__ + 1) == d_stateStack__.size())
    {
        size_t newSize = d_stackIdx__ + STACK_EXPANSION;
        d_stateStack__.resize(newSize);
        d_valueStack__.resize(newSize);
    }
    ++d_stackIdx__;
    d_stateStack__[d_stackIdx__] = d_state__ = state;
    *(d_vsp__ = &d_valueStack__[d_stackIdx__]) = d_val__;
}

void parserBase::popToken__()
{
    d_token__ = d_nextToken__;

    d_val__ = d_nextVal__;
    d_nextVal__ = STYPE__();

    d_nextToken__ = _UNDETERMINED_;
}
     
void parserBase::pushToken__(int token)
{
    d_nextToken__ = d_token__;
    d_nextVal__ = d_val__;
    d_token__ = token;
}
     
void parserBase::pop__(size_t count)
{
    if (d_stackIdx__ < static_cast<int>(count))
    {
        ABORT();
    }

    d_stackIdx__ -= count;
    d_state__ = d_stateStack__[d_stackIdx__];
    d_vsp__ = &d_valueStack__[d_stackIdx__];
}

inline size_t parserBase::top__() const
{
    return d_stateStack__[d_stackIdx__];
}

void parser::executeAction(int production)
try
{
    if (d_token__ != _UNDETERMINED_)
        pushToken__(d_token__);     // save an already available token

                                    // save default non-nested block $$
    if (int size = s_productionInfo[production].d_size)
        d_val__ = d_vsp__[1 - size];

    switch (production)
    {
        // $insert actioncases
        
        case 1:
        {
         
         result_.swap(d_vsp__[0].data<Tag__::ANY>());
         }
        break;

        case 2:
        {
         
         result_ = d_vsp__[0].data<Tag__::SEQ>();
         }
        break;

        case 3:
        {
         
         result_ = d_vsp__[0].data<Tag__::SEQ>();
         }
        break;

        case 4:
        {
         if (d_scanner.matched() == "#t") d_val__.get<Tag__::ANY>() = true; else d_val__.get<Tag__::ANY>() = false;
         }
        break;

        case 5:
        {
         
         d_val__.get<Tag__::ANY>() = static_cast<decof::integer>(std::stoi(d_scanner.matched()));
         }
        break;

        case 6:
        {
         
         d_val__.get<Tag__::ANY>() = static_cast<decof::real>(std::stod(d_scanner.matched()));
         }
        break;

        case 7:
        {
         
         decof::string res = d_scanner.matched();
         res.resize(backslash_escape_decoder(res.cbegin() + 1, res.cend() - 1, res.begin()));
         d_val__.get<Tag__::ANY>() = res;
         }
        break;

        case 8:
        {
         
         d_val__.get<Tag__::ANY>() = base64decode(d_scanner.matched().substr(1));
         }
        break;

        case 9:
        {
         
         d_val__.get<Tag__::SEQ>();
         }
        break;

        case 10:
        {
         
         d_val__.get<Tag__::SEQ>().swap(d_vsp__[0].data<Tag__::SEQ>());
         }
        break;

        case 11:
        {
         
         d_val__.get<Tag__::SEQ>().push_back(d_vsp__[0].data<Tag__::ANY>());
         }
        break;

        case 12:
        {
         
         d_vsp__[-2].data<Tag__::SEQ>().push_back(d_vsp__[0].data<Tag__::ANY>());
         d_val__.get<Tag__::SEQ>().swap(d_vsp__[-2].data<Tag__::SEQ>());
         }
        break;

        case 13:
        {
         
         d_val__.get<Tag__::SEQ>().swap(d_vsp__[-1].data<Tag__::SEQ>());
         }
        break;

        case 14:
        {
         
         d_val__.get<Tag__::SEQ>().swap(d_vsp__[-1].data<Tag__::SEQ>());
         }
        break;

    }
}
catch (std::exception const &exc)
{
    exceptionHandler__(exc);
}

inline void parserBase::reduce__(PI__ const &pi)
{
    d_token__ = pi.d_nonTerm;
    pop__(pi.d_size);

}

// If d_token__ is _UNDETERMINED_ then if d_nextToken__ is _UNDETERMINED_ another
// token is obtained from lex(). Then d_nextToken__ is assigned to d_token__.
void parser::nextToken()
{
    if (d_token__ != _UNDETERMINED_)        // no need for a token: got one
        return;                             // already

    if (d_nextToken__ != _UNDETERMINED_)
    {
        popToken__();                       // consume pending token
    }
    else
    {
        ++d_acceptedTokens__;               // accept another token (see
                                            // errorRecover())
        d_token__ = lex();
        if (d_token__ <= 0)
            d_token__ = _EOF_;
    }
    print();
}

// if the final transition is negative, then we should reduce by the rule
// given by its positive value. Note that the `recovery' parameter is only
// used with the --debug option
int parser::lookup(bool recovery)
{
// $insert threading
    SR__ *sr = s_state[d_state__];          // get the appropriate state-table
    int lastIdx = sr->d_lastIdx;            // sentinel-index in the SR__ array
    
    SR__ *lastElementPtr = sr + lastIdx;
    lastElementPtr->d_token = d_token__;    // set search-token
    
    SR__ *elementPtr = sr + 1;              // start the search at s_xx[1]
    while (elementPtr->d_token != d_token__)
        ++elementPtr;
    

    if (elementPtr == lastElementPtr)   // reached the last element
    {
        if (elementPtr->d_action < 0)   // default reduction
        {
            return elementPtr->d_action;                
        }

        // No default reduction, so token not found, so error.
        throw UNEXPECTED_TOKEN__;
    }

    // not at the last element: inspect the nature of the action
    // (< 0: reduce, 0: ACCEPT, > 0: shift)

    int action = elementPtr->d_action;


    return action;
}

    // When an error has occurred, pop elements off the stack until the top
    // state has an error-item. If none is found, the default recovery
    // mode (which is to abort) is activated. 
    //
    // If EOF is encountered without being appropriate for the current state,
    // then the error recovery will fall back to the default recovery mode.
    // (i.e., parsing terminates)
void parser::errorRecovery()
try
{
    if (d_acceptedTokens__ >= d_requiredTokens__)// only generate an error-
    {                                           // message if enough tokens 
        ++d_nErrors__;                          // were accepted. Otherwise
        error("Syntax error");                  // simply skip input

    }


    // get the error state
    while (not (s_state[top__()][0].d_type & ERR_ITEM))
    {
        pop__();
    }

    // In the error state, lookup a token allowing us to proceed.
    // Continuation may be possible following multiple reductions,
    // but eventuall a shift will be used, requiring the retrieval of
    // a terminal token. If a retrieved token doesn't match, the catch below 
    // will ensure the next token is requested in the while(true) block
    // implemented below:

    int lastToken = d_token__;                  // give the unexpected token a
                                                // chance to be processed
                                                // again.

    pushToken__(_error_);                       // specify _error_ as next token
    push__(lookup(true));                       // push the error state

    d_token__ = lastToken;                      // reactivate the unexpected
                                                // token (we're now in an
                                                // ERROR state).

    bool gotToken = true;                       // the next token is a terminal

    while (true)
    {
        try
        {
            if (s_state[d_state__]->d_type & REQ_TOKEN)
            {
                gotToken = d_token__ == _UNDETERMINED_;
                nextToken();                    // obtain next token
            }
            
            int action = lookup(true);

            if (action > 0)                 // push a new state
            {
                push__(action);
                popToken__();

                if (gotToken)
                {

                    d_acceptedTokens__ = 0;
                    return;
                }
            }
            else if (action < 0)
            {
                // no actions executed on recovery but save an already 
                // available token:
                if (d_token__ != _UNDETERMINED_)
                    pushToken__(d_token__);
 
                                            // next token is the rule's LHS
                reduce__(s_productionInfo[-action]); 
            }
            else
                ABORT();                    // abort when accepting during
                                            // error recovery
        }
        catch (...)
        {
            if (d_token__ == _EOF_)
                ABORT();                    // saw inappropriate _EOF_
                      
            popToken__();                   // failing token now skipped
        }
    }
}
catch (ErrorRecovery__)       // This is: DEFAULT_RECOVERY_MODE
{
    ABORT();
}

    // The parsing algorithm:
    // Initially, state 0 is pushed on the stack, and d_token__ as well as
    // d_nextToken__ are initialized to _UNDETERMINED_. 
    //
    // Then, in an eternal loop:
    //
    //  1. If a state does not have REQ_TOKEN no token is assigned to
    //     d_token__. If the state has REQ_TOKEN, nextToken() is called to
    //      determine d_nextToken__ and d_token__ is set to
    //     d_nextToken__. nextToken() will not call lex() unless d_nextToken__ is 
    //     _UNDETERMINED_. 
    //
    //  2. lookup() is called: 
    //     d_token__ is stored in the final element's d_token field of the
    //     state's SR_ array. 
    //
    //  3. The current token is looked up in the state's SR_ array
    //
    //  4. Depending on the result of the lookup() function the next state is
    //     shifted on the parser's stack, a reduction by some rule is applied,
    //     or the parsing function returns ACCEPT(). When a reduction is
    //     called for, any action that may have been defined for that
    //     reduction is executed.
    //
    //  5. An error occurs if d_token__ is not found, and the state has no
    //     default reduction. Error handling was described at the top of this
    //     file.

int parser::parse()
try 
{
    push__(0);                              // initial state
    clearin();                              // clear the tokens.

    while (true)
    {
        try
        {
            if (s_state[d_state__]->d_type & REQ_TOKEN)
                nextToken();                // obtain next token


            int action = lookup(false);     // lookup d_token__ in d_state__

            if (action > 0)                 // SHIFT: push a new state
            {
                push__(action);
                popToken__();               // token processed
            }
            else if (action < 0)            // REDUCE: execute and pop.
            {
                executeAction(-action);
                                            // next token is the rule's LHS
                reduce__(s_productionInfo[-action]); 
            }
            else 
                ACCEPT();
        }
        catch (ErrorRecovery__)
        {
            errorRecovery();
        }
    }
}
catch (Return__ retValue)
{
    return retValue;
}

// $insert namespace-close
}

}
