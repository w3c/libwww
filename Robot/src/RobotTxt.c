/*
**	@(#) $Id$
**	
**	W3C Webbot can be found at "http://www.w3.org/Robot/"
**	
**	Copyright © 1995-1998 World Wide Web Consortium, (Massachusetts
**	Institute of Technology, Institut National de Recherche en
**	Informatique et en Automatique, Keio University). All Rights
**	Reserved. This program is distributed under the W3C's Software
**	Intellectual Property License. This program is distributed in the hope
**	that it will be useful, but WITHOUT ANY WARRANTY; without even the
**	implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
**	PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
**	details.
**
**  Authors:
**	JP		John Punin
**
**  History:
**	Oct 1998	Written
*/

#include "HTRobMan.h"
#include "RobotTxt.h"

PUBLIC UserAgent * new_user_agent (void)
{
    UserAgent * ua;
    if ((ua = (UserAgent *) HT_CALLOC(1, sizeof(UserAgent))) == NULL)
        HT_OUTOFMEM("new_user_agent");
    ua->disallow = HTList_new();
    return ua;
}

PUBLIC char * get_name_user_agent (UserAgent * ua)
{
    return ua ? ua->name : NULL;
}

PUBLIC HTList * get_disallow_user_agent (UserAgent * ua)
{
    return ua ? ua->disallow : NULL;
}

PUBLIC BOOL set_name_user_agent (UserAgent * ua, char * name)
{
    if (ua && name) {
	StrAllocCopy(ua->name, name);
	return YES;
    }
    return NO;
}

PUBLIC BOOL add_disallow_user_agent (UserAgent * ua, char * disallow)
{
    if (ua && disallow) {
	char * da = NULL;
	StrAllocCopy(da, disallow);

	/* @@@ Should be an association list (HTAssocList) @@@ */
	return HTList_addObject(ua->disallow, da);
    }
    return NO;
}

PUBLIC BOOL delete_user_agent (UserAgent * ua)
{
    if (ua) {
	HT_FREE(ua->name);
	if (ua->disallow) {
	    HTList *cur = ua->disallow;
	    char *pres;
	    while ((pres = (char *) HTList_nextObject(cur)))
		HT_FREE(pres);
	    HTList_delete(ua->disallow);
	}
	return YES;
    }
    return NO;
}

PUBLIC BOOL delete_all_user_agents(HTList *user_agents)
{
    if (user_agents) {
	HTList *cur = user_agents;
	UserAgent *pres;
	while ((pres = (UserAgent *) HTList_nextObject(cur)))
	    delete_user_agent(pres);
	return HTList_delete(user_agents);
    }
    return NO;
}

PUBLIC char * get_regular_expression (HTList * user_agents, char * name_robot)
{
    if (user_agents && name_robot) {
	HTChunk *ch = HTChunk_new (1024);
	HTList *cur = user_agents;
	UserAgent *pres;
	UserAgent *ua_gen=NULL;
	int found=0;

	while ((pres = (UserAgent *) HTList_nextObject(cur))) {
	    char *name = get_name_user_agent(pres);

	    if(!strcmp(name,"*"))
		ua_gen = pres;

	    if(!strcmp(name,name_robot)) {
		put_string_disallow(ch,pres);
		found = 1;
	    }
	}
	if(!found && ua_gen) put_string_disallow(ch,ua_gen);

	return (HTChunk_toCString (ch));
    }
    return NULL;
}

PUBLIC BOOL put_string_disallow (HTChunk * ch, UserAgent * ua)
{
    if (ch && ua) {
	HTList *cur = get_disallow_user_agent(ua);
	char *pres;
	int first = 1;
  
	while ((pres = (char *) HTList_nextObject(cur))) {
	    if(!first)
		HTChunk_puts (ch,"|");
	    else
		first = 0;
	    HTChunk_puts (ch,pres);
	}
	return YES;
    }
    return NO;
}

PUBLIC void print_user_agent(UserAgent *ua)
{
  HTList *cur = ua->disallow;
  char *pres;
  HTTRACE(APP_TRACE, "User Agent : %s \n" _ ua->name);
  while ((pres = (char*) HTList_nextObject(cur)))
      HTTRACE(APP_TRACE, "Disallow : %s \n" _ pres);
}

PUBLIC void print_all_user_agents(HTList * user_agents)
{
    HTList *cur = user_agents;
    UserAgent *pres;
    while ((pres = (UserAgent *) HTList_nextObject(cur)))
    {
	HTTRACE(APP_TRACE, "\nNew User Agent\n");
	print_user_agent(pres);
    }
}

PUBLIC HTList * get_all_user_agents(char * rob_str)
{
    if (rob_str) {
	char * ptr = rob_str;
	HTList * user_agents = HTList_new();

	/* skip blank spaces */
	while(isspace((int)*ptr))
	    ptr++;

	/* skip comments */
	ptr = skip_comments(ptr);

	if(!get_user_agents(ptr,user_agents))
	    HTTRACE(APP_TRACE, "Something is wrong in robots.txt\n");

	return user_agents;
    }
    return NULL;
}

PUBLIC char * skip_comments(char *ptr)
{
    if (ptr && *ptr == '#') {
	do {
	    while(*ptr != '\n')
		ptr++;
	    while(isspace((int)*ptr))
		ptr++;
	} while (*ptr == '#');
    }
    return ptr;
}

PUBLIC void scan_name_until_eoline(char *robot_str, char *name)
{
  char *ptr = robot_str;
  char *ntr = name;
  while(*ptr != '\n' && *ptr != '#')
    {
      *ntr = *ptr;
      ntr++; ptr++;
      if(*ptr == '\0')
	break;
    }
  *ntr = '\0';
}

PUBLIC void scan_name_until_space(char *robot_str, char *name)
{
  char *ptr = robot_str;
  char *ntr = name;
  while(!isspace((int)*ptr) && *ptr != '#')
    {
      *ntr = *ptr;
      ntr++; ptr++;
      if(*ptr == '\0')
	break;
    }
  *ntr = '\0';
}

PUBLIC BOOL get_user_agents(char * ptr, HTList *user_agents)
{
  char *uastr = "user-agent:";
  char *disstr = "disallow:";
  int luastr = 10;
  int ldisstr = 9;
  char name[2000];
  int indices[200];
  int i = 0;
  if (ptr && !strncasecomp(ptr,uastr,luastr)) {
      UserAgent *ua = NULL;
      do {
	i=0;
	do {
	  ua = new_user_agent();
	  HTList_appendObject(user_agents,(void *)ua);
	  indices[i++] = HTList_indexOf(user_agents, (void *)ua);
	  ptr += luastr + 1;
	  while(isspace((int)*ptr))
	    ptr++;
	  scan_name_until_eoline(ptr,name); 
	  ptr += strlen(name) + 1;
	  while(isspace((int)*ptr))
	    ptr++;
	  ptr = skip_comments(ptr);
	  set_name_user_agent(ua,name);
	} while(!strncasecomp(ptr,uastr,luastr));

	if(!strncasecomp(ptr, disstr,ldisstr))
	  {
	    do {
	      ptr += ldisstr + 1;
	      scan_name_until_space(ptr,name); 
	      ptr += strlen(name) + 1;
	      while(isspace((int)*ptr))
		ptr++;
	      ptr = skip_comments(ptr);
	      if(i==1)
		add_disallow_user_agent(ua,name);
	      else
		{
		  int j;
		  for(j = 0 ; j < i ; j++)
		    {
		      ua = HTList_objectAt(user_agents, indices[j]);
		      add_disallow_user_agent(ua,name);
		    }
		}
	    } while(!strncasecomp(ptr,disstr,ldisstr));
	  }
	else
	  return NO;

      } while(!strncasecomp(ptr,uastr,luastr));
      return YES;
    }
  else
    return NO;
}

PUBLIC char * scan_robots_txt(char *rob_str, char *name_robot)
{
  char *reg_exp_exclude = NULL;
  HTList * user_agents = get_all_user_agents(rob_str);
  /*print_all_user_agents(user_agents);*/

  reg_exp_exclude = get_regular_expression(user_agents, name_robot);
  delete_all_user_agents(user_agents);

  return reg_exp_exclude;
}

#ifdef ROBOTS_TXT_STANDALONE

int 
main(int argc, char *argv[])
{
  char *text;
  char *reg_exp;
  char *filename= argc > 1 ? argv[1] : "robots.txt";
  FILE *fp;
  struct stat statb;
  /* make sure the file is a regular text file and open it */
  if(stat(filename, &statb) == -1 ||
     (statb.st_mode & S_IFMT ) != S_IFREG ||
     !(fp = fopen(filename, "r"))) 
    {
      if((statb.st_mode & S_IFMT) == S_IFREG)
	perror(filename);
      else
	HTTRACE(ALL_TRACE, "%s : not a regular file \n" _ filename);
      return 1;
    }

  if(!(text = malloc((unsigned)(statb.st_size +1))))
    {
      HTTRACE(ALL_TRACE, "Can't alloc enough space for %s" _ filename);
      fclose(fp);
      return;
    }
  if(!fread(text,sizeof(char), statb.st_size + 1, fp))
    HTTRACE(APP_TRACE, "Warning: may not have read entire file!\n");
  text[statb.st_size] = 0; /* be sure to NULL-terminate */
  fclose(fp);
  if(argc > 2)
    {
      reg_exp = scan_robots_txt(text,argv[2]);
      if(reg_exp)
	{
	  HTTRACE(ALL_TRACE, "REG EXP : %s \n" _ reg_exp);
	  free(reg_exp);
	}
    }
  free(text);

  return 0;
}


#endif
