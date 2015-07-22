Souvent, lorsque vous rencontrez des problèmes de configuration de serveur après un match, c'est que votre fichier "server.cfg" est incomplet.

Voici un fichier "server.cfg" complet. En adaptant ce fichier à votre serveur, vous augmentez vos chances de correctement rétablir la configuration par défaut de votre serveur.

```
// Infos
hostname "Nom du serveur"
sv_contact "Votre e-mail"
sv_lan 0 // (1) LAN (0) Web
sv_region 3 // (0) USA East Cost, (1) USA West Cot, (2) South America, (3) Europe, (4) Asia, (5) Australia, (6) Orient, (7) Africa, (255) the World
// sv_visiblemaxplayers 17


// Sécurité
rcon_password "Votre mot de passe RCON"
sv_password "" // Mot du passe du server ("" <=> pas de mot de passe)
sv_rcon_banpenalty 0
sv_rcon_minfailures 5
sv_rcon_minfailuretime 30
sv_rcon_maxfailures 10


// Téléchargements
// sv_downloadurl "votre-mirroir/cstrike"
sv_allowdownload 1


// Logs
log on
sv_logbans 1
sv_logecho 1
sv_logfile 1
sv_log_onefile 1


// Fichiers de configurations supplémentaires
exec banned_user.cfg
exec banned_ip.cfg
exec mani_server.cfg


// Réglages pour un tickrate 66
sv_allowupload 1
sv_minupdaterate 66
sv_maxupdaterate 66
sv_minrate 30000
sv_maxrate 35000
sv_mincmdrate 66
sv_maxcmdrate 66
decalfrequency 10
fps_max 0


// Configuration du jeu
mp_allowspectators 1
mp_autocrosshair 0
mp_autokick 0
mp_autoteambalance 1
mp_buytime 1.5
mp_c4timer 35
mp_chattime 6
mp_fadetoblack 0
mp_flashlight 1
mp_footsteps 1
mp_forcecamera 0
mp_fraglimit 0
mp_freezetime 0
mp_friendlyfire 1
mp_hostagepenalty 0
mp_limitteams 0
mp_maxrounds 0
mp_playerid 0
mp_roundtime 3
mp_spawnprotectiontime 0
mp_startmoney 800
mp_timelimit 20
mp_tkpunish 1
mp_weaponstay 1
mp_winlimit 0
sv_airaccelerate 10
sv_airmove 1
sv_alltalk 1
sv_cheats 0
sv_clienttrace 1
sv_consistency 1
sv_gravity 800
sv_maxspeed 320
sv_pausable 0
sv_timeout 120
sv_voiceenable 1
phys_pushscale 1
phys_timescale 1

// Si vous avez "ma_war 1" dans votre config de match
ma_war 0

// Si vous avez "mattie_eventscripts 0" dans votre config de match
mattie_eventscripts 1
```