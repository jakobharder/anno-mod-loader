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

??? tip "Merge multiple flags at once"
    `<ModValue />` with `Merge` and `Remove` can be used multiple times in any order.

    === ":material-pillar: 117"
        ```xml
        <ModOp GUID="6626" Merge="Region/AllowedWeather">
          <AllowedWeather>
            <ModValue Remove="Rain;Snow" />
            <ModValue Merge="Custom" />
            <ModValue Remove="SnowRain" />
          </AllowedWeather>
        </ModOp>
        ```
    === ":material-xml: Result"
        ```diff hl_lines="2-3"
          <Region>
        -   <AllowedWeather>Rain</AllowedWeather>
        +   <AllowedWeather>Custom</AllowedWeather>
          </Region>
        ```

## Insert Calculations

{{a117}}

`<ModValue Insert/>` and `<ModOp Content/>` support XPath 1.0 functions.

For example use `number()` to add to a number instead of replacing it.

=== ":material-pillar: 117"
    ```xml
    <ModOp GUID="1010343"
      Replace="Residence7/ResidentMax"
      Content="number(Residence7/ResidentMax) + 2">
      <ResidentMax><ModOpContent /></ResidentMax>
    </ModOp>
    ```

=== ":material-animation-outline: 117 & 1800 ⚠️"
    ```xml
    <!-- number table -->
    <ModOp Type="add" GUID="1010343" Path="/Values">
      <Number>1</Number>
      <!-- ... -->
      <Number>100</Number>
    </ModOp>
    <!-- add your value and pick from the table -->
    <ModOp Type="replace" GUID="1010343"
      Path="/Values/Residence7/ResidentMax"
      Content="~/Values/Number[number(../Residence7/ResidentMax) + 2]/text()">
      <ResidentMax><ModOpContent /></ResidentMax>
    </ModOp>
    <!-- remove table -->
    <ModOp Type="remove" GUID="1010343" Path="/Values/Number" />
    ```

More examples with `ModValue`.

=== ":material-pillar: 117"
    ```xml
    <!-- addition -->
    <ModOp Property="Maintenance" Merge="Workforce">
      <Workforce><ModValue Insert="self::node() + 10" /><!-- (1)! --></Workforce>
    </ModOp>

    <!-- division -->
    <ModOp Property="Storage" Merge="Amount">
      <Amount><ModValue Insert="round(self::node() div 2)" /><!-- (2)! --></Amount>
    </ModOp>
    ```

    1.  Or shorter: `Insert=". + 10"`.

    2.  Make sure to round to full numbers.

Available operators: `+`, `-`, `*`, `div`, `mod`.

Available calculation related XPath functions: `number()`, `count()`, `position()`, `round()`.

!!! info "Checkout the [XPath Cheatsheet](https://devhints.io/xpath) to learn more about XPath."

## Insert Options

{{a117}}

[Option values](../options.md) can be used in `<ModValue Insert/>` and `<ModOp Content/>` as values to insert.

--8<-- "./options.md:use-as-value"
