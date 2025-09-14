# Item Lists

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
