# ModOp Conditions

You can use conditions to skip ModOps.
The ModOp will only be executed when nodes with the path in `Condition` are found - or none exist if you start with `!`.

```xml
<ModOp Type="addNextSibling" GUID="190886"
       Condition="!//Values[Standard/GUID='1500010714']">
  <Asset>
    <Template>ItemEffectTargetPool</Template>
    <Values>
      <Standard>
        <GUID>1500010714</GUID>
        <Name>all sand mines</Name>
      </Standard>
      <ItemEffectTargetPool>
        <EffectTargetGUIDs />
      </ItemEffectTargetPool>
    </Values>
  </Asset>
</ModOp>
```

## Condition with GUID

Use `~` if the condition should be relative to the `GUID` your ModOp is operating.

```xml
<ModOp Type="add" GUID="800111"
  Condition="!~/Values/ExpeditionFeature/FeedOption/FeedOptions[Item/Product='1500010120']"
  Path="/Values/ExpeditionFeature/FeedOption/FeedOptions">
  <Item>
    <MoraleFactor>0.1</MoraleFactor>
    <Product>1500010120</Product>
  </Item>
</ModOp>
```

## Conditions on Include and Group

Conditions also work for includes and groups.

```xml
<Include File="/products/cheese/assets.include.xml"
         Condition="!//Values[Standard/GUID='1500010102']" />

<Group Condition="!//Values[Standard/GUID='1500010102']">
  <ModOp />
</Group>
```

## Best Practice: Prefer Negative Conditions

The following two ModOps will lead to the same result, but have a stark difference.

```xml
<!-- negative condition -->
<ModOp Type="add" GUID="800111"
  Condition="!~/ExpeditionFeature/FeedOption/FeedOptions[Item/Product='1500010120']"
  Path="/Values/ExpeditionFeature/FeedOption/FeedOptions" />

<!-- positive condition -->
<ModOp Type="add" GUID="800111"
  Condition="~/ExpeditionFeature/FeedOption/FeedOptions[not(Item/Product='1500010120')]"
  Path="/Values/ExpeditionFeature/FeedOption/FeedOptions" />
```

If GUID `800111` is missing, or `FeedOption` has been emoved from it, then the negative condition be true and the ModOp throws a warning because of a not found node.

On the other hand, the positive condition will skip the ModOp without any warning as the condition was not met.

## Disable warnings with `AllowNoMatch`

⚠ It's better to use conditions in many cases, as conditions still allow some warnings when there are issues.

Disable match not found warnings.
Useful if you want to add items, but only if they don't exist.

Adding a product only once to a list with `AllowNoMatch`:
```xml
<ModOp Type="add" GUID="120055"
       Path="/Values/ProductStorageList/ProductList[not(Item/Product='1500010102')]"
       AllowNoMatch="1">
  <Item>
    <Product>1500010102</Product>
  </Item>
</ModOp>
```
