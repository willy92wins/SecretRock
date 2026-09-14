modded class Hologram
{
    protected bool SecretRock_IsKitProjection()
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                return true;
            }
        }

        return false;
    }

    override string ProjectionBasedOnParent()
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                return deployKit.GetDeployedClassname();
            }
        }

        return super.ProjectionBasedOnParent();
    }

    override string GetProjectionName(ItemBase item)
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                return deployKit.GetDeployedClassname();
            }
        }

        return super.GetProjectionName(item);
    }

    override EntityAI PlaceEntity(EntityAI entity_for_placing)
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                return entity_for_placing;
            }
        }

        return super.PlaceEntity(entity_for_placing);
    }

    override void SetProjectionPosition(vector position)
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                vector depOffset = deployKit.GetDeployPositionOffset();
                vector depFinal = position + depOffset;

                if (m_Projection)
                {
                    m_Projection.SetPosition(depFinal);
                }
                return;
            }
        }

        super.SetProjectionPosition(position);
    }

    override vector GetDefaultOrientation()
    {
        if (m_Parent)
        {
            SecretRock_KitBaseDeployable deployKit = SecretRock_KitBaseDeployable.Cast(m_Parent);
            if (deployKit)
            {
                vector baseOri = super.GetDefaultOrientation();
                vector oriOff = deployKit.GetDeployOrientationOffset();
                vector oriResult = baseOri + oriOff;
                return oriResult;
            }
        }

        return super.GetDefaultOrientation();
    }

    override bool IsColliding()
    {
        if (SecretRock_IsKitProjection())
        {
            return false;
        }
        return super.IsColliding();
    }

    override void EvaluateCollision(ItemBase action_item)
    {
        if (SecretRock_IsKitProjection())
        {
            bool bNoCollide = false;
            SetIsColliding(bNoCollide);
            return;
        }
        super.EvaluateCollision(action_item);
    }

    override bool IsCollidingAngle()
    {
        if (SecretRock_IsKitProjection())
        {
            return false;
        }
        return super.IsCollidingAngle();
    }

    override bool IsFloating()
    {
        if (SecretRock_IsKitProjection())
        {
            return false;
        }
        return super.IsFloating();
    }

    override void RefreshVisual()
    {
        if (SecretRock_IsKitProjection())
        {
            SetSelectionToRefresh("zbytek");
        }
        super.RefreshVisual();
    }
};
