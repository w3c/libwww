/* Implementation dependent include files */
#include "tcp.h"

/* Library include files */
#include "HTUtils.h"
#include "HTAccess.h"
#include "HTError.h"
#include "HTThread.h"
#include "HTList.h"
#include "HTEvent.h"
#include "HTThread.h" 

PUBLIC void HTEventThread_new ARGS1(void *, new_net) 
{
    HTThread_new((HTNetInfo *) new_net);
}

PUBLIC int HTEventThread_clear ARGS1( void *, old_net) 
{
    return HTThread_clear((HTNetInfo *) old_net);
}

PUBLIC void HTEventThreadState ARGS2( SOCKFD, sockfd, HTThreadAction, action) 
{
    HTThreadState( sockfd, action ) ;
}    

    
    

    
