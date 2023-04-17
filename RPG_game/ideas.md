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

