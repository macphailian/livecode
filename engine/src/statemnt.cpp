/* Copyright (C) 2003-2013 Runtime Revolution Ltd.

This file is part of LiveCode.

LiveCode is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License v3 as published by the Free
Software Foundation.

LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

#include "prefix.h"

#include "globdefs.h"
#include "filedefs.h"
#include "objdefs.h"
#include "parsedef.h"

#include "uidc.h"
#include "execpt.h"
#include "hndlrlst.h"
#include "handler.h"
#include "scriptpt.h"
#include "statemnt.h"
#include "object.h"
#include "chunk.h"
#include "param.h"
#include "mcerror.h"
#include "debug.h"
#include "util.h"
#include "parentscript.h"

#include "globals.h"

#include "syntax.h"
#include "redraw.h"

////////////////////////////////////////////////////////////////////////////////

MCStatement::MCStatement()
{
	line = pos = 0;
	next = NULL;
}

MCStatement::~MCStatement()
{}

Parse_stat MCStatement::parse(MCScriptPoint &sp)
{
	initpoint(sp);
	return PS_NORMAL;
}

Exec_stat MCStatement::exec(MCExecPoint &ep)
{
	MCExecContext ctxt(ep);
	exec_ctxt(ctxt);
	if (!ctxt . HasError())
        return ctxt . GetExecStat();
	return ctxt . Catch(line, pos);
}

void MCStatement::exec_ctxt(MCExecContext&)
{
	fprintf(stderr, "ERROR: exec method for statement not implemented properly\n");
}

uint4 MCStatement::linecount()
{
	return 1;
}

uint4 MCStatement::countlines(MCStatement *stmp)
{
	uint4 count = 0;
	while (stmp != NULL)
	{
		count += stmp->linecount();
		stmp = stmp->getnext();
	}
	return count;
}

void MCStatement::deletestatements(MCStatement *start)
{
	MCStatement *stmp;
	while (start != NULL)
	{
		stmp = start;
		start = start->getnext();
		delete stmp;
	}
}

void MCStatement::deletetargets(MCChunk **targets)
{
	MCChunk *t1ptr = *targets;
	*targets = NULL;
	while (t1ptr != NULL)
	{
		MCChunk *t2ptr = t1ptr;
		t1ptr = t1ptr->next;
		delete t2ptr;
	}
}

Parse_stat MCStatement::gettargets(MCScriptPoint &sp, MCChunk **targets,
                                   Boolean forset)
{
	Boolean needchunk = False;
	MCChunk *pptr = NULL;
	while (True)
	{
		if (sp.skip_token(SP_COMMAND, TT_ELSE, S_UNDEFINED) == PS_NORMAL)
		{
			sp.backup();
			return PS_NORMAL;
		}
		Symbol_type type;
		switch (sp.next(type))
		{
		case PS_NORMAL:
			sp.backup();
			break;
		case PS_ERROR:
			return PS_ERROR;
		case PS_EOL:
		case PS_EOF:
			if (needchunk)
			{
				MCperror->add(PE_STATEMENT_BADCHUNK, sp);
				return PS_ERROR;
			}
			return PS_NORMAL;
		default:
			sp.backup();
			return PS_NORMAL;
		}
		MCChunk *newptr = new MCChunk(forset);
		if (newptr->parse(sp, False) != PS_NORMAL)
		{
			delete newptr;
			MCperror->add(PE_STATEMENT_BADCHUNK, sp);
			return PS_ERROR;
		}
		if (pptr == NULL)
			*targets = pptr = newptr;
		else
		{
			pptr->next = newptr;
			pptr = pptr->next;
		}
		if (sp.skip_token(SP_COMMAND, TT_ELSE, S_UNDEFINED) == PS_NORMAL
		        || sp.skip_token(SP_FACTOR, TT_TO, PT_TO) == PS_NORMAL)
		{
			sp.backup();
			return PS_NORMAL;
		}
		switch (sp.skip_token(SP_FACTOR, TT_BINOP, O_AND))
		{
		case PS_NORMAL:
			break;
		case PS_EOL:
		case PS_EOF:
			return PS_NORMAL;
		default:
			MCperror->add(PE_STATEMENT_NOTAND, sp);
			return PS_ERROR;
		}
		needchunk = True;
	}
	return PS_NORMAL;
}

Parse_stat MCStatement::getparams(MCScriptPoint &sp, MCParameter **params)
{
	Boolean needparam = False;
	MCParameter *pptr = NULL;
	while (True)
	{
		if (sp.skip_token(SP_COMMAND, TT_ELSE, S_UNDEFINED) == PS_NORMAL)
		{
			sp.backup();
			return PS_NORMAL;
		}
		Symbol_type type;
		switch (sp.next(type))
		{
		case PS_NORMAL:
			sp.backup();
			break;
		case PS_ERROR:
			return PS_ERROR;
		case PS_EOL:
		case PS_EOF:
			if (needparam)
			{
				MCperror->add(PE_STATEMENT_BADPARAM, sp);
				return PS_ERROR;
			}
			return PS_NORMAL;
		default:
			sp.backup();
			return PS_NORMAL;
		}
		MCParameter *newptr = new MCParameter;
		if (newptr->parse(sp) != PS_NORMAL)
		{
			delete newptr;
			MCperror->add(PE_STATEMENT_BADPARAM, sp);
			return PS_ERROR;
		}
		if (pptr == NULL)
			*params = pptr = newptr;
		else
		{
			pptr->setnext(newptr);
			pptr = newptr;
		}
		if (sp.skip_token(SP_COMMAND, TT_ELSE, S_UNDEFINED) == PS_NORMAL)
		{
			sp.backup();
			return PS_NORMAL;
		}
		switch (sp.next(type))
		{
		case PS_NORMAL:
			break;
		case PS_EOL:
		case PS_EOF:
			return PS_NORMAL;
		default:
			MCperror->add(PE_STATEMENT_NOTSEP, sp);
			return PS_ERROR;
		}
		if (type != ST_SEP)
		{
			MCperror->add(PE_STATEMENT_BADSEP, sp);
			return PS_ERROR;
		}
		needparam = True;
	}
	return PS_NORMAL;
}

Parse_stat MCStatement::getmods(MCScriptPoint &sp, uint2 &mstate)
{
	Symbol_type type;
	const LT *te;

	while (True)
	{
		if (sp.next(type) != PS_NORMAL)
		{
			MCperror->add(PE_STATEMENT_NOKEY, sp);
			return PS_ERROR;
		}
		if (sp.lookup(SP_FACTOR, te) != PS_NORMAL || te->type != TT_FUNCTION)
		{
			MCperror->add(PE_STATEMENT_BADKEY, sp);
			return PS_ERROR;
		}
		switch (te->which)
		{
		case F_COMMAND_KEY:
			mstate |= MS_CONTROL;
			break;
		case F_CONTROL_KEY:
			mstate |= MS_MAC_CONTROL;
			break;
		case F_OPTION_KEY:
			mstate |= MS_MOD1;
			break;
		case F_SHIFT_KEY:
			mstate |= MS_SHIFT;
			break;
		default:
			MCperror->add(PE_STATEMENT_BADKEY, sp);
			return PS_ERROR;
		}
		if (sp.skip_token(SP_COMMAND, TT_ELSE, S_UNDEFINED) == PS_NORMAL)
		{
			sp.backup();
			return PS_NORMAL;
		}
		switch (sp.next(type))
		{
		case PS_NORMAL:
			break;
		case PS_EOL:
		case PS_EOF:
			return PS_NORMAL;
		default:
			MCperror->add(PE_STATEMENT_BADSEP, sp);
			return PS_ERROR;
		}
		if (type != ST_SEP)
		{
			MCperror->add(PE_STATEMENT_BADSEP, sp);
			return PS_ERROR;
		}
	}
	return PS_NORMAL;
}

Parse_stat MCStatement::gettime(MCScriptPoint &sp, MCExpression **in,
                                Functions &units)
{
	if (sp.skip_token(SP_FACTOR, TT_IN) == PS_NORMAL)
	{
		if (sp.parseexp(False, True, in) != PS_NORMAL)
		{
			MCperror->add
			(PE_STATEMENT_BADINEXP, sp);
			return PS_ERROR;
		}
		if (sp.skip_token(SP_FACTOR, TT_FUNCTION, F_MILLISECS) == PS_NORMAL)
			units = F_MILLISECS;
		else
			if (sp.skip_token(SP_FACTOR, TT_FUNCTION, F_SECONDS) == PS_NORMAL
			        || sp.skip_token(SP_FACTOR, TT_CHUNK, CT_SECOND) == PS_NORMAL)
				units = F_SECONDS;
			else
				if (sp.skip_token(SP_FACTOR, TT_FUNCTION, F_TICKS) == PS_NORMAL)
					units = F_TICKS;
				else
					units = F_TICKS;
	}
	return PS_NORMAL;
}

void MCStatement::initpoint(MCScriptPoint &sp)
{
	line = sp.getline();
	pos = sp.getpos();
}

void MCStatement::compile(MCSyntaxFactoryRef ctxt)
{
	MCSyntaxFactoryBeginStatement(ctxt, line, pos);
	MCSyntaxFactoryExecUnimplemented(ctxt);
	MCSyntaxFactoryEndStatement(ctxt);
};


////////////////////////////////////////////////////////////////////////////////

#ifdef _MOBILE
extern bool MCIsPlatformMessage(MCNameRef handler_name);
extern Exec_stat MCHandlePlatformMessage(MCNameRef p_message, MCParameter *p_parameters);
#endif

MCComref::MCComref(MCNameRef n)
{
	/* UNCHECKED */ MCNameClone(n, name);
	handler = nil;
	params = NULL;
	resolved = false;
    platform_message = false;
}

MCComref::~MCComref()
{
	while (params != NULL)
	{
		MCParameter *tmp = params;
		params = params->getnext();
		delete tmp;
	}
	MCNameDelete(name);
}

Parse_stat MCComref::parse(MCScriptPoint &sp)
{
	initpoint(sp);
	if (getparams(sp, &params) != PS_NORMAL)
	{
		MCperror->add(PE_STATEMENT_BADPARAMS, sp);
		return PS_ERROR;
	}
#ifdef _MOBILE
    if (MCIsPlatformMessage(name))
    {
        platform_message = true;
        resolved = true;
    }
#endif
    
	return PS_NORMAL;
}

#if /* MCComref::exec */ LEGACY_EXEC
Exec_stat MCComref::exec(MCExecPoint &ep)
{
	if (MCscreen->abortkey())
	{
		MCeerror->add(EE_HANDLER_ABORT, line, pos);
		return ES_ERROR;
	}

	if (!resolved)
	{
		// MW-2008-01-28: [[ Inherited parentScripts ]]
		// If we are in parentScript context, then the object we search for
		// private handlers in is the parentScript's object, rather than the
		// ep's.
		MCParentScriptUse *t_parentscript;
		t_parentscript = ep . getparentscript();

		MCObject *t_object;
		if (t_parentscript == NULL)
			t_object = ep . getobj();
		else
			t_object = t_parentscript -> GetParent() -> GetObject();

		// MW-2008-10-28: [[ ParentScripts ]] Private handlers are resolved
		//   relative to the object containing the handler we are executing.
		MCHandler *t_resolved_handler;
		t_resolved_handler = t_object -> findhandler(HT_MESSAGE, name);
		if (t_resolved_handler != NULL && t_resolved_handler -> isprivate())
			handler = t_resolved_handler;

		resolved = true;
    }
    
	Exec_stat stat;
    MCExecContext ctxt(ep);
	MCParameter *tptr = params;
	while (tptr != NULL)
	{
		MCVariable* t_var;
		t_var = tptr -> evalvar(ep);
        
		if (t_var == NULL)
		{
			tptr -> clear_argument();
			while ((stat = tptr->eval(ep)) != ES_NORMAL && (MCtrace || MCnbreakpoints) && !MCtrylock && !MClockerrors)
				MCB_error(ctxt, line, pos, EE_STATEMENT_BADPARAM);
			if (stat != ES_NORMAL)
			{
				MCeerror->add(EE_STATEMENT_BADPARAM, line, pos);
				return ES_ERROR;
			}
			tptr->set_argument(ep);
		}
		else
			tptr->set_argument_var(t_var);
        
		tptr = tptr->getnext();
        
	}
	MCObject *p = ep.getobj();
	MCExecContext *oldctxt = MCECptr;
	MCECptr = &ctxt;
	stat = ES_NOT_HANDLED;
	Boolean added = False;
	if (MCnexecutioncontexts < MAX_CONTEXTS)
	{
		ep.setline(line);
		MCexecutioncontexts[MCnexecutioncontexts++] = &ctxt;
		added = True;
	}
    
#ifdef _MOBILE
    if (platform_message)
    {
        stat = MCHandlePlatformMessage(name, params);
    }
#endif
    
	if (handler != nil)
	{
        // MW-2008-10-28: [[ ParentScripts ]] If we are in the context of a
        //   parent, then use a special method.
        if (ep . getparentscript() == NULL)
            stat = p -> exechandler(handler, params);
        else
            stat = p -> execparenthandler(handler, params, ep . getparentscript());
        
        switch(stat)
        {
            case ES_ERROR:
            case ES_PASS:
                MCeerror->add(EE_STATEMENT_BADCOMMAND, line, pos, handler -> getname());
                if (MCerrorptr == NULL)
                    MCerrorptr = p;
                stat = ES_ERROR;
                break;
                
            case ES_EXIT_HANDLER:
                stat = ES_NORMAL;
                break;
                
            default:
                break;
        }
	}
	else
	{
		stat = MCU_dofrontscripts(HT_MESSAGE, name, params);
		Boolean olddynamic = MCdynamicpath;
		MCdynamicpath = MCdynamiccard != NULL;
		if (stat == ES_PASS || stat == ES_NOT_HANDLED)
			switch (stat = p->handle(HT_MESSAGE, name, params, p))
			{
			case ES_ERROR:
			case ES_NOT_FOUND:
			case ES_NOT_HANDLED:
			case ES_PASS:
				MCeerror->add(EE_STATEMENT_BADCOMMAND, line, pos, name);
				stat = ES_ERROR;
				break;
			case ES_EXIT_HANDLER:
				stat = ES_NORMAL;
				break;
			default:
				break;
			}
		MCdynamicpath = olddynamic;
	}
	MCECptr = oldctxt;
	if (added)
		MCnexecutioncontexts--;
	return stat;
}
#endif

void MCComref::exec_ctxt(MCExecContext& ctxt)
{
    MCKeywordsExecCommand(ctxt, resolved, handler, params, name, line, pos, platform_message, false);
}

////////////////////////////////////////////////////////////////////////////////

Exec_stat MCKeywordsExecuteStatements(MCExecContext& ctxt, MCStatement *p_statements, Exec_errors p_error, bool p_is_try)
{
    Exec_stat stat = ES_NORMAL;
    MCStatement *tspr = p_statements;
    while (tspr != NULL)
    {
        if (MCtrace || MCnbreakpoints)
        {
            MCB_trace(ctxt, tspr->getline(), tspr->getpos());
            if (MCexitall)
                break;
        }
        ctxt . GetEP() . setline(tspr->getline());
        
        stat = tspr->exec(ctxt . GetEP());
        
        // MW-2011-08-17: [[ Redraw ]] Flush any screen updates.
        MCRedrawUpdateScreen();
        
        switch(stat)
        {
            case ES_NORMAL:
                if (MCexitall)
                    return ES_NORMAL;
                tspr = tspr->getnext();
                break;
            case ES_NEXT_REPEAT:
                tspr = NULL;
                break;
            case ES_EXIT_REPEAT:
                return stat;
            case ES_EXIT_SWITCH:
                return ES_NORMAL;
            case ES_ERROR:
                if ((MCtrace || MCnbreakpoints) && !MCtrylock && !MClockerrors)
                    do
                    {
                        ctxt . IgnoreLastError();
                        MCB_error(ctxt, tspr->getline(), tspr->getpos(),
                                  EE_REPEAT_BADSTATEMENT);
                    }
                while (MCtrace && (stat = tspr->exec(ctxt . GetEP())) != ES_NORMAL);
                if (stat == ES_ERROR)
                {
                    if (MCexitall)  
                        return p_is_try ? ES_NORMAL : ES_ERROR;
                    
                    ctxt . LegacyThrow(p_error);
                    return stat;
                }
                else
                    tspr = tspr->getnext();
                break;
            default:
                return stat;
        }
    }
    return stat;
}

void MCKeywordsExecCommand(MCExecContext& ctxt, bool resolved, MCHandler *handler, MCParameter *params, MCNameRef name, uint2 line, uint2 pos, bool platform_message, bool is_function)
{
	if (MCscreen->abortkey())
	{
		ctxt . LegacyThrow(EE_HANDLER_ABORT);
		return;
	}
    
	if (!resolved)
	{
		// MW-2008-01-28: [[ Inherited parentScripts ]]
		// If we are in parentScript context, then the object we search for
		// private handlers in is the parentScript's object, rather than the
		// ep's.
		MCParentScriptUse *t_parentscript;
		t_parentscript = ctxt . GetParentScript();
        
		MCObject *t_object;
		if (t_parentscript == NULL)
			t_object = ctxt . GetObject();
        else
            t_object = t_parentscript -> GetParent() -> GetObject();
        
        // MW-2008-10-28: [[ ParentScripts ]] Private handlers are resolved
        //   relative to the object containing the handler we are executing.
        MCHandler *t_resolved_handler;
		t_resolved_handler = t_object -> findhandler(is_function ? HT_FUNCTION : HT_MESSAGE, name);
		if (t_resolved_handler != NULL && t_resolved_handler -> isprivate())
			handler = t_resolved_handler;
        
        resolved = true;
    }

    if (is_function)
        MCexitall = False;
    
	// Go through all the parameters to the function, if they are not variables, clear their current value. Each parameter stores an expression
	// which allows its value to be re-evaluated in a given context. Re-evaluate each in the context of ep and set it to the new value.
	// As the ep should contain the context of the caller at this point, the expression should be evaluated in that context.
    Exec_stat stat;
	MCParameter *tptr = params;
	while (tptr != NULL)
	{
		MCVariable* t_var;
		t_var = tptr -> evalvar(ctxt);
        
		if (t_var == NULL)
		{
			tptr -> clear_argument();
            MCAutoValueRef t_value;
			if (!ctxt . TryToEvaluateParameter(tptr, line, pos, is_function ? EE_FUNCTION_BADSOURCE : EE_STATEMENT_BADPARAM, &t_value))
                return;
			tptr->setvalueref_argument(*t_value);
		}
		else
			tptr->set_argument_var(t_var);
        
        tptr = tptr->getnext();
    }
	MCObject *p = ctxt . GetObject();
	MCExecContext *oldctxt = MCECptr;
	MCECptr = &ctxt;
	stat = ES_NOT_HANDLED;
	Boolean added = False;
	if (MCnexecutioncontexts < MAX_CONTEXTS)
	{
		ctxt . GetEP() . setline(line);
		MCexecutioncontexts[MCnexecutioncontexts++] = &ctxt;
		added = True;
	}
    
#ifdef _MOBILE
    if (platform_message)
    {
        stat = MCHandlePlatformMessage(name, params);
    }
#endif
    
	if (handler != nil)
	{
        // MW-2008-10-28: [[ ParentScripts ]] If we are in the context of a
        //   parent, then use a special method.
        if (ctxt . GetParentScript() == nil)
            stat = p -> exechandler(handler, params);
        else
            stat = p -> execparenthandler(handler, params, ctxt . GetParentScript());
        
        switch(stat)
        {
            case ES_ERROR:
            case ES_PASS:
                MCeerror->add(is_function ? EE_FUNCTION_BADFUNCTION : EE_STATEMENT_BADCOMMAND, line, pos, handler -> getname());
                if (MCerrorptr == NULL)
                    MCerrorptr = p;
                stat = ES_ERROR;
                break;
                
            case ES_EXIT_HANDLER:
                stat = ES_NORMAL;
                break;
                
            default:
                break;
        }
	}
	else
	{
		stat = MCU_dofrontscripts(is_function ? HT_FUNCTION : HT_MESSAGE, name, params);
		Boolean olddynamic = MCdynamicpath;
		MCdynamicpath = MCdynamiccard != NULL;
		if (stat == ES_PASS || stat == ES_NOT_HANDLED)
        {
            if (is_function)
            {
                // PASS STATE FIX
                Exec_stat oldstat = stat;
                stat = p->handle(HT_FUNCTION, name, params, p);
                if (oldstat == ES_PASS && stat == ES_NOT_HANDLED)
                    stat = ES_PASS;
            }
            else
            {
                switch (stat = p->handle(HT_MESSAGE, name, params, p))
                {
                    case ES_ERROR:
                    case ES_NOT_FOUND:
                    case ES_NOT_HANDLED:
                    case ES_PASS:
                        MCeerror->add(EE_STATEMENT_BADCOMMAND, line, pos, name);
                        stat = ES_ERROR;
                        break;
                    case ES_EXIT_HANDLER:
                        stat = ES_NORMAL;
                        break;
                    default:
                        break;
                }
            }
        }
            
		MCdynamicpath = olddynamic;
	}
	MCECptr = oldctxt;
	if (added)
		MCnexecutioncontexts--;
	ctxt . SetExecStat(stat);
}