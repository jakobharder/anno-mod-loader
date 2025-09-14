# Copy Existing Content

## Copy Existing Nodes with `Content`

```xml
<ModOp Type="replace" GUID="1500010225"
       Path="/Properties/Building/InfluencedVariationDirection"
       Content="//Values[Standard/GUID='1500010200']/Building/InfluencedVariationDirection" />
```

## Wrap ModOp `Content`

```xml
<ModOp Type="merge" Path="@1010399/AssetPool/AssetList"
  Content="@120055/ProductStorageList/ProductList/Item/Product/text()">
  <Item>
    <Asset>
      <ModOpContent />
    </Asset>
  </Item>
</ModOp>
```

Content source:

```xml
<ProductList>
  <Item>
    <Product>120008</Product>
  </Item>
  <!-- ... -->
</ProductList>
```

Merge target:

```xml
<AssetPool>
  <AssetList>
    <Item>
      <Asset>120008</Asset>
    </Item>
    <!-- ... -->
  </AssetList>
</AssetPool>
```