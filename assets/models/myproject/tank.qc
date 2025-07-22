$scale 1
$modelname	"/models/myproject/tank.mdl"
$cd "C:\Users\theto\Documents\Github\aegis\External\models\myproject"
$body tank "tank"
$cdtexture "C:\Users\theto\Documents\Github\aegis\External\models\myproject"
$cliptotextures
$sequence idle "tank"    loop fps 30 ACT_IDLE 1

$hboxset "barrel"
$hbox 0 "Tank" -113.20 89.59 204.24 113.11 854.06 493.96 0.00 0.00 0.00

$hboxset "body"
$hbox 0 "TankRoot" -7.90 -1.00 -13.88 219.16 1.00 -11.88 0.00 0.00 0.00

$hboxset "head"
$hbox 0 "TankHead" -99.29 -134.65 -142.70 142.38 -130.62 -138.63 0.01 0.00 -45.03

$include	"tank_wwmt_custom.qci"
