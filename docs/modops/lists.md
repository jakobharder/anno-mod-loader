# Item Lists

## Normal Merge

{{all}}

A normal `Merge` updates individual elements (i.e. `Item`) by order of occurrence.

=== ":material-animation-outline: 117 & 1800"
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
=== ":material-file-code-outline: Input"
    ```xml
    <Maintenance>
      <Maintenances>
        <Item>
          <Product>1010017</Product>
          <Amount>400</Amount>
          <InactiveAmount>200</InactiveAmount>
        </Item>
        <Item>
          <Product>1010117</Product>
          <Amount>150</Amount>
          <ShutdownThreshold>0.5</ShutdownThreshold>
        </Item>
      </Maintenances>
    </Maintenance>
    ```
=== ":material-xml: Result"
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
    -     <Product>1010117</Product>
    -     <Amount>150</Amount>
    +     <Product>1010367</Product>
    +     <Amount>50</Amount>
          <ShutdownThreshold>0.5</ShutdownThreshold>
        </Item>
      </Maintenances>
    </Maintenance>
    ```

!!! warning "Relying on index is prone to compatibility issues."

## ModItem Merge

{{a117}}

`<ModItem Merge="Attribute">` merges items out of order, or adds them when there's no match.
The item is merged with the first item that matches the attribute in `Merge`.

=== ":material-pillar: 117"
    ```xml hl_lines="2"
    <ModOp GUID="114365" Merge="Product/ProductionRegions">
      <ModItem Merge="RegionType">
        <RegionType>Moderate</RegionType>
      </ModItem>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml hl_lines="2"
    <ModOp Type="add" GUID="114365"
      Condition="!/Values/Product/ProductionRegions[Item/RegionType='Moderate']"
      Path="/Values/Product/ProductionRegions">
      <Item>
        <RegionType>Moderate</RegionType>
      </Item>
    </ModOp>
    ```

??? tip "You can replace the merge selecting attribute."
    The merge selecting attribute can be replaced as well.
    Use `<ModItem Merge="Attribute='Value'">` to select the item.

    === ":material-pillar: 117"
        ```xml hl_lines="3"
        <ModOp GUID="100780" Merge="Maintenance">
          <Maintenances>
            <ModItem Merge="Product='1010117'">
              <Product>1010367</Product>
              <Amount>50</Amount>
            </ModItem>
          </Maintenances>
        </ModOp>
        ```
    === ":material-file-code-outline: Input"
        ```xml hl_lines="8-12"
        <Maintenance>
          <Maintenances>
            <Item>
              <Product>1010017</Product>
              <Amount>400</Amount>
              <InactiveAmount>200</InactiveAmount>
            </Item>
            <Item>
              <Product>1010117</Product>
              <Amount>150</Amount>
              <ShutdownThreshold>0.5</ShutdownThreshold>
            </Item>
          </Maintenances>
        </Maintenance>
        ```
    === ":material-xml: Result"
        ```diff
        <Maintenance>
          <Maintenances>
            <Item>
              <Product>1010017</Product>
              <Amount>400</Amount>
              <InactiveAmount>200</InactiveAmount>
            </Item>
            <Item>
        -     <Product>1010117</Product>
        -     <Amount>150</Amount>
        +     <Product>1010367</Product>
        +     <Amount>50</Amount>
              <ShutdownThreshold>0.5</ShutdownThreshold>
            </Item>
          </Maintenances>
        </Maintenance>
        ```

### Append Behavior

{{a117}}

By default, missing items are added to the end of the item list.
You can change that by defining a `Append` or `Prepend` path.

The default is `Append='last()'`.

=== ":material-pillar: 117"
    ```xml hl_lines="2"
    <ModOp GUID="502017" Merge="ProductList/List">
      <ModItem Merge="Product" Append="Product='1010200'">
        <Product>1500010836</Product>
      </ModItem>
    </ModOp>
    ```

=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp Type="addNextSibling" GUID="502017"
      Condition="!~/Values/ProductList/List/Item[Product='1500010836']"
      Path="/Values/ProductList/List/Item[Product='1010200']">
      <Item>
        <Product>1500010836</Product>
      </Item>
    </ModOp>
    ```

## Insert with Wrapper

{{a117}}

With `ModValue` in combination `ModItem` + `ModValueContent` you can construct lists with automatic duplicate handling.

=== ":material-pillar: 117"
    ```xml hl_lines="5 6 7"
    <!-- .. -->
    <ItemEffectTargetPool>
      <ModValue Insert="@191455/ItemEffectTargetPool/EffectTargetGUIDs/Item" />
      <ModValue Insert="@191458/ItemEffectTargetPool/EffectTargetGUIDs/Item/GUID">
        <ModItem Merge="GUID">
          <ModValueContent />
        </ModItem>
      </ModValue>
    </ItemEffectTargetPool>
    <!-- .. -->
    ```

### SkipParent

{{a117}}

Use `SkipParent` if you need to exclude the top-level parent, because you have items with multiple elements.

`SkipParent` also works with `ModOpContent`.

=== ":material-pillar: 117"
    ```xml hl_lines="6"
    <!-- .. -->
    <ItemEffectTargetPool>
      <ModValue Insert="@191455/ItemEffectTargetPool/EffectTargetGUIDs/Item" />
      <ModValue Insert="@191458/ItemEffectTargetPool/EffectTargetGUIDs/Item">
        <ModItem Merge="GUID">
          <ModValueContent SkipParent="1" />
        </ModItem>
      </ModValue>
    </ItemEffectTargetPool>
    <!-- .. -->
    ```