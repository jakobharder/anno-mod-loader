# Dynamic Content

## Copy Existing Nodes

{{all}}

=== ":material-at: 117 Inline"
    ```xml
    <ModOp Replace="@1500010225/Building/InfluencedVariationDirection">
      <ModValue Insert="@1500010200/Building/InfluencedVariationDirection">
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Replace="@1500010225/Building/InfluencedVariationDirection"
           Content="@1500010200/Building/InfluencedVariationDirection" />
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp Type="replace" GUID="1500010225"
           Path="/Values/Building/InfluencedVariationDirection"
           Content="@1500010200/Building/InfluencedVariationDirection" />
    ```

## Wrap Copied Nodes

{{all}}

=== ":material-at: 117 Inline"
    ```xml hl_lines="5"
    <ModOp Replace="@1010399/AssetPool/AssetList">
      <ModValue Insert="@120055/ProductStorageList/ProductList/Item/Product/text()">
        <Item>
          <Asset>
            <ModValueContent />
          </Asset>
        </Item>
      </ModValue>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml hl_lines="5"
    <ModOp Replace="@1010399/AssetPool/AssetList"
           Content="@120055/ProductStorageList/ProductList/Item/Product/text()">
      <Item>
        <Asset>
          <ModOpContent />
        </Asset>
      </Item>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800"
    ```xml
    <ModOp Type="replace" GUID="1010399"
           Path="/Values/AssetPool/AssetList"
           Content="@120055/ProductStorageList/ProductList/Item/Product/text()">
      <Item>
        <Asset>
          <ModOpContent />
        </Asset>
      </Item>
    </ModOp>
    ```
=== "Input"
    ```xml hl_lines="3"
    <ProductList>
      <Item>
        <Product>120008</Product>
      </Item>
      <!-- ... -->
    </ProductList>
    ```
=== "Result"
    ```xml hl_lines="3"
    <AssetList>
      <Item>
        <Asset>120008</Asset>
      </Item>
      <!-- ... -->
    </AssetList>
    ```

## Insert Local Content

{{a117}}

Use `<ModValue Insert="<local path>" />` to copy data from a local path without specifying `GUID`.

=== ":material-pillar: 117"
    ```xml
    <ModOp Merge="@123">
      <Inline><ModValue Insert="../Standard/GUID/text()" /></Inline>
    </ModOp>
    <ModOp Property="ItemSocketSet" Merge="../ExpeditionAttribute">
      <FluffText><ModValue Insert="../../ItemSocketSet/SetBuff/text()" /></FluffText>
    </ModOp>
    ```

=== ":material-animation-outline: 117 & 1800 ⚠️"
    Previously, copying local content was only possible in combination with a specific `GUID`.

    ```xml
    <ModOp Type="add" GUID="123" Content="Standard/GUID/text()">
      <GUID><ModOpContent /></GUID>
    </ModOp>
    ```

## Merge Flags

{{a117}}

Use `<ModValue Merge="Your;Flags" />` to insert one or more flags if not already present, instead of overwriting the existing flags value.
Similarily use `Remove` to remove flags.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="114365" Merge="Product">
      <AssociatedRegion><ModValue Merge="Moderate" /></AssociatedRegion>
    </ModOp>
    ```
    ```xml
    <ModOp GUID="114365" Merge="Product">
      <AssociatedRegion><ModValue Remove="Moderate" /></AssociatedRegion>
    </ModOp>
    ```
=== ":material-at: 117"
    ```xml
    <ModOp Merge="@114365/Product">
      <AssociatedRegion><ModValue Merge="Moderate" /></AssociatedRegion>
    </ModOp>
    ```
    ```xml
    <ModOp Merge="@114365/Product">
      <AssociatedRegion><ModValue Remove="Moderate" /></AssociatedRegion>
    </ModOp>
    ```
=== ":material-animation-outline: 117 & 1800 ⚠️"
    ```xml
    <ModOp Type="add" GUID="114365"
      Condition="!/Values/Product/AssociatedRegion[contains(text(), 'Moderate')]"
      Path="/Values/Product/AssociatedRegion">;Moderate</ModOp>
    ```
    ```xml
    <!-- a generic remove was not possible -->
    ```

## Insert Calculations

{{a117}}

=== ":material-pillar: 117"
    ```xml
    <!-- addition -->
    <ModOp Property="Maintenance" Merge="Workforce">
      <Workforce><ModValue Insert="number(self::node()) + 10" /></Workforce>
    </ModOp>

    <!-- division -->
    <ModOp Property="Storage" Merge="Amount">
      <Amount><ModValue Insert="(number(self::node()) - number(self::node()) mod 2) div 2" /></Amount>
    </ModOp>
    ```

Available operators: `+`, `-`, `*`, `div`, `mod`