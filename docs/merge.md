# Merge ModOp

## Merge is order independent

The order of nodes do not matter anymore for successful merges.
Before you had to have the exact same order as the game.

```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <AllowChangeDirection>1</AllowChangeDirection>
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Building>
-     <AllowChangeVariation>0</AllowChangeVariation>
-     <AllowChangeDirection>0</AllowChangeDirection>
+     <AllowChangeVariation>1</AllowChangeVariation>
+     <AllowChangeDirection>1</AllowChangeDirection>
    </Building>
  </Values>
</Asset>
```

## Merge adds missing nodes

```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
  <BuildModeStartVariation>0</BuildModeStartVariation>
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Building>
-     <AllowChangeVariation>0</AllowChangeVariation>
+     <AllowChangeVariation>1</AllowChangeVariation>
+     <BuildModeStartVariation>0</BuildModeStartVariation>
    </Building>
  </Values>
</Asset>
```

## Merge does not remove nodes

```xml
<ModOp GUID="123" Type="merge" Path="/Values">
  <Building />
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Building>
      <AllowChangeVariation>1</AllowChangeVariation>
    </Building>
  </Values>
</Asset>
```

Use `replace` or `remove` instead if you want to remove content.

## Merge supports lists

```xml
<ModOp Type="merge" GUID="100780" Path="/Values/Maintenance">
  <Maintenances>
    <Item>
      <Product>1010017</Product>
      <Amount>50000</Amount>
      <InactiveAmount>30000</InactiveAmount>
    </Item>
    <Item>
      <Product>1010367</Product>
      <Amount>50</Amount>
    </Item>
  </Maintenances>
</ModOp>
```

Result:

```diff
<Maintenance>
  <Maintenances>
    <Item>
      <Product>1010017</Product>
-     <Amount>400</Amount>
-     <InactiveAmount>200</InactiveAmount>
+     <Amount>50000</Amount>
+     <InactiveAmount>30000</InactiveAmount>
    </Item>
    <Item>
-     <Product>1010367</Product>
-     <Amount>50</Amount>
+     <Product>1010117</Product>
+     <Amount>150</Amount>
      <ShutdownThreshold>0.5</ShutdownThreshold>
    </Item>
  </Maintenances>
</Maintenance>
```

But be aware, relying on index is prone to compatibility issues.

## Merge can be used like `add` or `replace`

The following to merge operations lead to the same result.

Used like add, will only add if not there already:
```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
</ModOp>
```

Used like replace including the last path element (i.e. `Building`):
```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <Building>
    <AllowChangeVariation>1</AllowChangeVariation>
  </Building>
</ModOp>
```

You need to use the second approach in some rare cases when parent and child element have the same name - which happens in localizations for example (`Texts/Text/Text/GUID`).
It only applies if there is exactly one top-level element in the ModOp that has the same name as the last path element.

*Note the following changed behavior since modloader10 / GU17.*

The following ModOp doesn't update `AllowChangeVariation` anymore.
Yes, it did before...
It will instead add the node to `Values`.

```xml
<ModOp GUID="123" Type="merge" Path="/Values/Building">
  <AllowChangeVariation>1</AllowChangeVariation>
  <Building>
    <BuildModeStartVariation>0</BuildModeStartVariation>
  </Building>
</ModOp>
```

Result:
```diff
<Asset>
  <Values>
    <Standard>
      <GUID>123</GUID>
    </Standard>
    <Building>
      <AllowChangeVariation>0</AllowChangeVariation>
+     <BuildModeStartVariation>0</BuildModeStartVariation>
+     <Building>
+       <AllowChangeVariation>1</AllowChangeVariation>
+     </Building>
    </Building>
  </Values>
</Asset>
```
