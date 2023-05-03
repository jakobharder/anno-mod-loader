# ModOp Examples

Other docs

- [Simple ModOps](./modop-guide.md)
- [Replace and Merge](./modop-replace-merge.md)
- [Conditions](./modop-conditions.md)

## Increase a number without overwriting already larger numbers

Example: Set harbor radius 40 but only if nobody did it before (i.e. Increased Harbor Area).

```xml
<ModOp Type="merge" GUID="2001241"
  Condition="!~/Values/BuildingBaseTiles[HarbourConstructionAreaRadius&gt;=40]"
  Path="/Values/BuildingBaseTiles[HarbourConstructionAreaRadius&lt;40]">
  <HarbourConstructionAreaRadius>40</HarbourConstructionAreaRadius>
</ModOp>
```

### Note 1

`&lt;` stands `<` and `&gt;` stands for `>`. You can use `<` and `>` directly, but that is not valid XML and you will get errors from VSCode.
The modloader accepts both.

### Note 2

The condition is negative with `!`. You could do a positive condition with the same path as `Path`:

```
Condition="~/Values/BuildingBaseTiles[HarbourConstructionAreaRadius&lt;40]"
```

The difference is: if there is any error in the Condition path itself - be it a typo or another mod removed that part - you don't get any warnings. The condition will just say: not available, hence don't execute.

On the other hand, the negative check with `!` skips if an conditions exists exactly how you expect it.
Meaning it will execute if there is something unexpected, leading to warnings in the end.

*Psalm 172, 3, Lion*

## Add yourself to all buffs another target has

Example: Custom guildhouse on water should get the same EffectTargets (i.e. from the palace) as the normal guildhouse.

```xml
<ModOp Type="add" Path="//ItemEffect/EffectTargets[Item/GUID='1010516']">
  <Item>
    <GUID>1554900077</GUID>
  </Item>
</ModOp>
```
