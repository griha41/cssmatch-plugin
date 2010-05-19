//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//

#include "icvar.h"
#include "convar.h"

class CPluginConVarAccessor : public IConCommandBaseAccessor
{
public:
	virtual bool	RegisterConCommandBase( ConCommandBase *pCommand )
	{
		// Link to engine's list
		g_pCVar->RegisterConCommand( pCommand );
		return true;
	}

};

CPluginConVarAccessor g_ConVarAccessor;
void InitCVars( CreateInterfaceFn cvarFactory )
{
	g_pCVar = (ICvar*)cvarFactory( CVAR_INTERFACE_VERSION, NULL );
	if ( g_pCVar )
	{
		ConVar_Register(0, &g_ConVarAccessor );
	}
}

