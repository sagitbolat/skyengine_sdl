# Character Creation:
* Base the game on a litrpg world
* Have classical attributes that the player levels up. Stat levels affect the user physically up to level [25].
  After level [25], stat changes start to affect the user magically.
    * Strength
        * A mainstat for damage with heavy weapons. 
        * Requirment for Strength checks.
        * Up to level [25]: Makes the user's physique stronger, more carry weight, 
          higher unarmed combat, etc.
        * Past level [25]: TBD
    * Dexterity
        * A mainstat for damage with skill-based weapons.
        * Requirement for Dexterity checks.
        * Increases speed (which determines hit order in combat).
        * Up to level [25]: TBD
        * Past level [25]: TBD
    * Intelligence
        * A mainstat for damage with spells and magic weapons
        * Requirement for Intelligence checks.
        * Increases xp gain
        * Up to level [25]: TBD
        * Past level [25]: TBD
    * Constitution
        * Does not have a level threshold. 
        * Increases health and health regen rates
    * Endurance
        * Does not have a level threshold. 
        * Increases stamina and stamina regen rates
    * Wisdom
        * Does not have a level threshold. 
        * Increases mana and mana regen rates
    * Perception
        * Up to level [25]: Physical changes. Increases sight range, hearing, and scent.
        * Past level [25]: Magical changes. Grants the ability to detect danger, see in the dark,
          sense living beings through walls, detect people's personalities and details about their character.
    * Charisma
        * Up to level [25]: Physical changes. Improved physical appearence. Makes it easier to barter with
          people who are attracted to your gender. Required for charisma checks in conversations.  
        * Past level [25]: Magical changes. Able to charm neutrals and enemies, thereby avoiding combat. 
          able to bypass gender romance requirements, giving wider options. 

* Have the player able to pick out of a set of distinct Gods to be their champion
    * Each God has unique modifiers (pros and cons)
    * God of War gives the champion:
        * Adaptive mainstat equal to the player's level. Adaptive mainstat becomes the artificial mainstat that 
          player has the most of.
            * EX: if the player is level 14, and has STR 12, DEX 15, INT 10, then the bonus Adaptive 
              mainstat will be +14 aritificial DEX.
            * Artificial stats only contribute to the stat checks and weapon damage, not to skill trees and 
              carry weight, speed, or xp gain.
        * Lower weapon level requirements. 
    * God of Luck gives the champion:
        * 3 extra attribute points each levelup 
        * Start the game with randomized skills.
        * Higher chance of loot drops.
        * Loot is randomly upgraded in rarity.
        * Increased dodge chance and crit hit chance
        * All choices are randomly assigned instead of giving the player the oportunity to choose
          EX: The stat distribution for each level up is randomly assigned.  
            * This is not truely random. Instead, the choices are done based on gamestate.
            * i.e. if a player is in certain situations, like on a quest that has stat checks, 
              those stats will be chosen during a levelup



# Combat System

## The Battlefield

### Version 1:
* The battlefield for each encounter is entered the same way a pokemon battle is entered.
    * A seperate sceen for the battle
    * The scene is a tile grid with enemies and player characters and summons. 
* This makes handling summons easier, as they always spawn during start of combat.
* Makes careful planning and maneuvaring the non-combat map less rewarding.
* Breaks the flow of gameplay
### Version 2:
* The battlefield is integrated in the level design. the map itself acts as the turnbased field where you continously take combat
    and non-combat actions. this is similar to most textbased roguelike combat systems.
* Things like summons can be automatically summoned when an enemy detects the player, or can be summoned manually? (explore this further)
* This makes combat vs exploration flow more naturally and allow the player to more strategically maneuver different levels.


## The Accuracy System:
* Every offensive action (spells, attacks, etc.) from both players and enemies have a base accuracy.
* An accuracy of 100 means the action is guaranteed to hit its target.
* An accuracy of below 100 (such as 50) will mean that the action will only hit the target at that percentage
    * An accuracy of 60 will mean that the action will "miss" 40% of the time.
* When an action misses, it instead hit a tile near the target tile. 
    * Example: A fireball spell fired at tile (10,10) that misses might hit tile (11, 10) or (11, 11) etc.
    * Some actions might have a bigger spread, meaning it might hit a tile some number of spaces away from the target
        while others will only hit tiles adjacent to the target tile.
* If accuracy exceeds 100 (either from buffs, skills, items, or the action have >100 base accuracy), we get surplus accuracy
    * Surplus accuracy gives a chance to score a critical hit. 
    * The chance to score a critical is calculated as (accuracy - 100).
        * Example: at 130 accuracy, the chance of a critical hit is 30%.
        * The damage of the critical hit depends on player stats and modifiers (action dependent)
            * Example: A fireball spell might get more critical damage from player's Intelligence and level of pyromancy proficiency.
            * Example: A dagger throw skill might get more critical damage from player's Luck

