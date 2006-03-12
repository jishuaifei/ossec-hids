/*   $OSSEC, decoder.c, v0.1, 2005/06/21, Daniel B. Cid$   */

/* Copyright (C) 2005 Daniel B. Cid <dcid@ossec.net>
 * All right reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */


/* v0.1: 2005/06/21
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os_regex/os_regex.h"
#include "os_xml/os_xml.h"

#include "shared.h"

#include "eventinfo.h"
#include "decoder.h"




/* DecodeEvent.
 * Will use the plugins to decode the received event.
 */
void DecodeEvent(Eventinfo *lf)
{
    PluginNode *node;
    PluginNode *child_node;
    PluginInfo *nnode;

    node = OS_GetFirstPlugin();

    /* Return if no node...
     * This shouldn't happen here anyways
     */
    if(!node)
        return;


    do 
    {
        if(node->plugin)
        {
            nnode = node->plugin;

            
            /* If prematch fails, go to the next plugin in the list */
            if(!nnode->prematch || !OSRegex_Execute(lf->log,nnode->prematch))
                continue;

            lf->log_tag = nnode->name;

            child_node = node->child;


            /* Setting the type */
            if(nnode->type)
            {
                lf->type = nnode->type;
            }


            /* Check if we have any child plugin */
            while(child_node)
            {
                nnode = child_node->plugin;

                
                /* If we have a pre match and it matches, keep
                 * going. If we don't have a pre match, keep going too.
                 */
                if(nnode->prematch)
                {     
                    if(OSRegex_Execute(lf->log,nnode->prematch))
                        break;
                }
                else
                    break;

                child_node = child_node->next;
                nnode = NULL;
            }

            if(!nnode)
                return;

            merror("regex for: %s", lf->log);

            /* Getting the regex */
            if(nnode->regex)
            {
                int i = 0;

                merror("here");
                /* If Regex does not match, return */
                if(!OSRegex_Execute(lf->log, nnode->regex))
                {
                    merror("faileD");
                    return;
                }
                merror("success");
               
                while(nnode->regex->sub_strings[i])
                {
                    
                    if(nnode->order[i])
                    {
                        nnode->order[i](lf, nnode->regex->sub_strings[i]);
                        nnode->regex->sub_strings[i] = NULL;
                        i++;
                        continue;
                    }

                    /* We do not free any memory used above */
                    os_free(nnode->regex->sub_strings[i]);
                    nnode->regex->sub_strings[i] = NULL;
                    i++;
                }
            }


            /* Checking if the FTS is set */
            if(nnode->fts)
            {
                lf->fts = nnode->fts;

                /* the comment we need to duplicate too */
                if(nnode->ftscomment)
                    lf->comment = nnode->ftscomment;
            }


            /* Matched  */
            return;         
        }

    }while((node=node->next) != NULL);
}


/*** Event decoders ****/
void *DstUser_FP(Eventinfo *lf, char *field)
{
    lf->dstuser = field;
    return(NULL);
}
void *User_FP(Eventinfo *lf, char *field)
{
    lf->user = field;
    return(NULL);
}
void *SrcIP_FP(Eventinfo *lf, char *field)
{
    lf->srcip = field;
    return(NULL);
}
void *DstIP_FP(Eventinfo *lf, char *field)
{
    lf->dstip = field;
    return(NULL);
}
void *SrcPort_FP(Eventinfo *lf, char *field)
{
    lf->srcport = field;
    return(NULL);
}
void *DstPort_FP(Eventinfo *lf, char *field)
{
    lf->dstport = field;
    return(NULL);
}
void *Protocol_FP(Eventinfo *lf, char *field)
{
    lf->protocol = field;
    return(NULL);
}
void *Action_FP(Eventinfo *lf, char *field)
{
    lf->action = field;
    return(NULL);
}
void *ID_FP(Eventinfo *lf, char *field)
{
    lf->id = field;
    return(NULL);
}
void *Url_FP(Eventinfo *lf, char *field)
{
    lf->url = field;
    return(NULL);
}
void *None_FP(Eventinfo *lf, char *field)
{
    free(field);
    return(NULL);
}


/* EOF */
