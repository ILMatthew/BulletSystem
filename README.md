# BulletSystem

Custom linetraced solution for simulating bullets.
A Bullet Manager is responsable for updating the location and direction of all bullets.
Bullets can also penetrate objects and ricochet off of surfaces. The angle of the impact and the size of the hit object are taken into account.

Bullet are described in a struct by their location, direction and type.
Bullet types are Data Assets. Different type of bullets can be created, with different mass, penetration, ecc.

On impact, bullet hole decal and particle are activated base on physic material.

Working on:
- Async sim: in order to have as much bullets on screen as possible, bullet simulation will be splitted on separate threads;
- Bullet visual: currently trying to use a Niagara system to represent the bullet around the player
